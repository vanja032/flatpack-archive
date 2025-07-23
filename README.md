# Flatpack Archive – Technical Specification

Flatpack is a binary archive format that packages an entire directory tree—including files and folders—into a single `.flatpack` file. The archive is composed of three main sections stored sequentially:

```
  ______ _       _                    _                        _     _             _______          _
 |  ____| |     | |                  | |        /\            | |   (_)           |__   __|        | |
 | |__  | | __ _| |_ _ __   __ _  ___| | __    /  \   _ __ ___| |__  ___   _____     | | ___   ___ | |
 |  __| | |/ _` | __| '_ \ / _` |/ __| |/ /   / /\ \ | '__/ __| '_ \| \ \ / / _ \    | |/ _ \ / _ \| |
 | |    | | (_| | |_| |_) | (_| | (__|   <   / ____ \| | | (__| | | | |\ V /  __/    | | (_) | (_) | |
 |_|    |_|\__,_|\__| .__/ \__,_|\___|_|\_\ /_/    \_\_|  \___|_| |_|_| \_/ \___|    |_|\___/ \___/|_|
                    | |
                    |_|
```

## Usage

```
flatpack create -i <input_folder> -o <output_file>
        [-c | --compress] [default: zlib]         (optional – enables compression)
        [-a <compression_type> | --algo <type>]   (optional – select: zlib/zstd/lz4/brotli)
        [-e <encryption_type> | --encrypt <encryption_type>] (optional – enables encryption, e.g. aes256)
        [-p <password> | --password <password>]   (optional – password for encryption)
        [-v | --verbose]                          (optional – verbose output)

flatpack extract -i <archive_file> -o <output_folder>
        [-p <password> | --password <password>]   (optional – password for decryption)
        [-v | --verbose]                          (optional – verbose output)

flatpack --help | -h
```

### Option Details

- `-i`, `--input` : Input folder (for create) or archive file (for extract) [required]
- `-o`, `--output` : Output file (for create) or output folder (for extract) [required]
- `-c`, `--compress` : Enable compression (default: zlib if not specified)
- `-a`, `--algo` : Compression algorithm: `zlib`, `lz4`, `zstd`, `brotli`
- `-e`, `--encrypt` : Encryption type (currently supported: `aes256`)
- `-p`, `--password` : Password for encryption/decryption (required if using encryption)
- `-v`, `--verbose` : Enable verbose output
- `-h`, `--help` : Show help message

### Examples

Create an archive with default compression (zlib):

```bash
flatpack create -i ./my_folder -o backup.flatpack -c
```

Create an archive with Brotli compression and AES-256 encryption:

```bash
flatpack create -i ./my_folder -o backup.flatpack -a brotli -e aes256 -p mypassword
```

Extract an encrypted archive:

```bash
flatpack extract -i backup.flatpack -o ./restored_folder -p mypassword
```

Show help:

```bash
flatpack -h
```

## Archive Format Overview

1. Header
2. File Table
3. Raw Data Section

Each section is defined precisely below.

### 1. Header

The **Header** is a fixed-size block that appears at the beginning of the archive. It contains identity, format versioning, and metadata describing how many entries exist in the archive.

**Header Layout**
| Field           | Size (bytes) | Description                                     |
| --------------- | ------------ | ----------------------------------------------- |
| `magic`         | 6            | ASCII characters `"FPACK"` with `\0` terminator |
| `major_version` | 1            | Major version of the archive format             |
| `minor_version` | 1            | Minor version                                   |
| `patch_version` | 1            | Patch version                                   |
| `file_count`    | 4            | Total number of entries in the file table       |

**Total Size: 13 bytes**

### 2. File Table

The **File Table** is an array of fixed-size entries. Each entry corresponds to one file or directory in the archive. The entries are laid out back-to-back with no delimiters.

**File Table Entry Layout**
| Field              | Size (bytes) | Description                                                                            |
| ------------------ | ------------ | -------------------------------------------------------------------------------------- |
| `path`             | 256          | UTF-8 null-terminated relative path (zero-padded)                                      |
| `offset`           | 8            | Offset (in bytes) to the start of the file data in archive                             |
| `size`             | 8            | Original uncompressed size (0 if entry is a directory)                                 |
| `compressed_size`  | 8            | Compressed size of the file data                                                       |
| `is_directory`     | 1            | Flag (1 = directory, 0 = file)                                                         |
| `compression_type` | 1            | Compression type used (0 = None, 1 = Zlib, 2 = LZ4, 3 = Zstd, 4 = Brotli, 5 = Default) |
| `encryption_type`  | 1            | Encryption type used (0 = None, 1 = AES256, 2 = Default)                               |

**Total Per Entry: 283 bytes**

### 3. Raw Data Section

This section holds the binary contents of files. File data is written sequentially in the same order that the entries appear in the file table.

- There are no delimiters or separators between file blocks.
- Directories have no data stored.
- Each file’s data can be found by seeking to `offset` and reading `compressed_size` bytes (decompress and/or decrypt as needed).

All raw data is stored as-is (uncompressed and unencrypted) unless compression and/or encryption is enabled.

## Storage Rules

- All numeric fields use little-endian encoding.
- Paths are relative to the folder provided at archive creation time.
- The format does not store absolute paths, permissions, timestamps, or file ownership.

## Extension Possibilities

The format is versioned and structured for forward compatibility. Future changes may include:

- Additional compression algorithms
- Additional encryption algorithms
- Checksums or hash digests for integrity
- Symbolic link or metadata support

## Installation & Usage

### Building the CLI

You can build the CLI in one of two ways:

#### Option 1: Manual CMake build

```bash
git clone https://github.com/vanja032/flatpack.git
cd flatpack
mkdir build && cd build
cmake ..
make
```

#### Option 2: Using build script

```bash
git clone https://github.com/vanja032/flatpack.git
cd flatpack
bash build.sh
```

### Installing the CLI

After building, install the CLI tool system-wide:

```bash
bash install.sh
```

This installs the `flatpack` binary to `/usr/local/bin`.

### Command-Line Usage

See the usage section above for details and examples.

## Supported Compression Types

You can specify the compression algorithm using `--algo` or `-a`. The available options are:

- `zlib`
- `lz4`
- `zstd`
- `brotli`

If no compression algorithm is specified for compression, files are stored compressed using **Default** algorithm _Zlib_.

## Supported Encryption Types

You can specify the encryption algorithm using `--encrypt` or `-e`. The available options are:

- `aes256`

If no encryption algorithm is specified, encryption is not used by default.

## License

This project is licensed under the MIT License.

## Author

Created and maintained by Vanja Sretenovic. For more information or contributions, visit [https://github.com/vanja032](https://github.com/vanja032).
