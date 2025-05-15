# Flatpack Archive – Technical Specification

Flatpack is a binary archive format that packages an entire directory tree—including files and folders—into a single `.flatpack` file. The archive is composed of three main sections stored sequentially:

1. Header
2. File Table
3. Raw Data Section

Each section is defined precisely below.

## 1. Header

The **Header** is a fixed-size block that appears at the beginning of the archive. It contains identity, format versioning, and metadata describing how many entries exist in the archive.

### Layout

| Field           | Size (bytes) | Description                                     |
| --------------- | ------------ | ----------------------------------------------- |
| `magic`         | 6            | ASCII characters `"FPACK"` with `\0` terminator |
| `major_version` | 1            | Major version of the archive format             |
| `minor_version` | 1            | Minor version                                   |
| `patch_version` | 1            | Patch version                                   |
| `file_count`    | 4            | Total number of entries in the file table       |

**Total Size: 13 bytes**

The header is written first, before any other section, and its presence and contents help validate the archive format during extraction.

## 2. File Table

The **File Table** is an array of fixed-size entries. Each entry corresponds to one file or directory in the archive. The entries are laid out back-to-back with no delimiters.

### File Entry Layout

| Field          | Size (bytes) | Description                                                |
| -------------- | ------------ | ---------------------------------------------------------- |
| `path`         | 256          | UTF-8 null-terminated relative path (zero-padded)          |
| `offset`       | 8            | Offset (in bytes) to the start of the file data in archive |
| `size`         | 8            | Number of bytes of file data (0 if entry is a directory)   |
| `is_directory` | 1            | Flag (1 = directory, 0 = file)                             |

**Total Per Entry: 273 bytes**

Each file or folder is stored as a single entry in this section. Only file entries have valid `offset` and `size` fields. Directory entries do not reference any data in the raw data section.

## 3. Raw Data Section

This section holds the binary contents of files. File data is written sequentially in the same order that the entries appear in the file table.

- There are no delimiters or separators between file blocks.
- Directories have no data stored.
- Each file’s data can be found by seeking to `offset` and reading `size` bytes.

All raw data is stored as-is (uncompressed), unless future extensions change this.

## Storage Rules

- All numeric fields use little-endian encoding.
- Paths are relative to the folder provided at archive creation time.
- The format does not store absolute paths, permissions, timestamps, or file ownership.

## Extension Possibilities

The format is versioned and structured for forward compatibility. Future changes may include:

- Compression flags or algorithms
- Optional encryption using password-based keys
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

To create an archive:

```bash
flatpack create -i ./my_folder -o backup.flatpack --compress --encrypt mypassword
```

To extract an archive:

```bash
flatpack extract -i backup.flatpack -o ./restored_folder --decrypt mypassword
```

For help:

```bash
flatpack --help
```

## License

This project is licensed under the MIT License.

## Author

Created and maintained by Vanja Sretenovic. For more information or contributions, visit [https://github.com/vanja032](https://github.com/vanja032).
