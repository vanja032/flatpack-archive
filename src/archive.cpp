// archive.cpp

#include "flatpack_archive/archive.hpp"
#include "flatpack_archive/compression.hpp"
#include "flatpack_archive/encryption.hpp"
#include "flatpack_archive/file_table.hpp"
#include "flatpack_archive/header.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace flatpack_archive {

namespace fs = std::filesystem;

void create_archive(const std::string &input_folder,
                    const std::string &output_file,
                    CompressionType compression_type,
                    EncryptionType encryption_type,
                    const std::string &password = "",
                    const bool verbose = false) {
  std::ofstream out(output_file, std::ios::binary);
  if (!out) {
    throw std::runtime_error("Failed to open archive file for writing");
  }

  FileTable file_table;

  for (const auto &entry : fs::recursive_directory_iterator(input_folder)) {
    FileEntry fe;
    std::string relative_path =
        fs::relative(entry.path(), input_folder).string();
    std::memset(fe.path, 0, sizeof(fe.path));
    std::memcpy(fe.path, relative_path.c_str(),
                std::min(sizeof(fe.path) - 1, relative_path.size()));

    fe.is_directory = entry.is_directory() ? 1 : 0;
    fe.offset = 0;
    fe.size = entry.is_regular_file() ? fs::file_size(entry.path()) : 0;
    fe.compressed_size = 0;
    fe.compression_type = CompressionType::None;
    fe.encryption_type = EncryptionType::None;
    file_table.add_entry(fe);
  }

  ArchiveHeader header;
  header.file_count = static_cast<uint32_t>(file_table.count());
  header.write(out);

  std::streampos table_start = out.tellp();
  file_table.write(out);

  std::vector<FileEntry> updated_entries;
  for (auto &fe : file_table.get_entries()) {
    if (fe.is_directory == 0) {
      fe.offset = static_cast<uint64_t>(out.tellp());

      std::ifstream in(fs::path(input_folder) / fe.path, std::ios::binary);
      std::vector<char> data(std::istreambuf_iterator<char>(in), {});

      std::vector<char> compressed_data;
      if (!compress(data, compressed_data, compression_type)) {
        compressed_data = data;
      } else {
        fe.compression_type = compression_type;
        if (verbose) {
          std::cout << "Compressed file: " << fe.path << " using "
                    << static_cast<int>(compression_type) << "\n";
        }
      }

      std::vector<char> encrypted_data;
      if (encrypt(compressed_data, encrypted_data, encryption_type, password)) {
        fe.encryption_type = encryption_type;
        if (verbose) {
          std::cout << "Encrypted file: " << fe.path << " using "
                    << static_cast<int>(encryption_type) << "\n";
        }
      } else {
        encrypted_data = compressed_data;
      }

      fe.compressed_size = encrypted_data.size();
      out.write(encrypted_data.data(), encrypted_data.size());
      if (verbose) {
        std::cout << "Stored file in archive: " << fe.path << "\n";
      }
    }
    updated_entries.push_back(fe);
  }

  out.seekp(table_start);
  for (const auto &fe : updated_entries) {
    fe.write(out);
  }
  out.close();
}

void extract_archive(const std::string &archive_file,
                     const std::string &output_folder,
                     const std::string &password = "",
                     const bool verbose = false) {
  std::ifstream in(archive_file, std::ios::binary);
  if (!in) {
    throw std::runtime_error("Failed to open archive file for reading");
  }

  ArchiveHeader header = ArchiveHeader::read(in);
  if (!header.is_valid()) {
    throw std::runtime_error("Invalid archive file format");
  }

  FileTable file_table = FileTable::read(in, header.file_count);

  for (const auto &fe : file_table.get_entries()) {
    std::filesystem::path out_path =
        std::filesystem::path(output_folder) / fe.path;

    if (fe.is_directory) {
      std::filesystem::create_directories(out_path);
    } else {
      std::filesystem::create_directories(out_path.parent_path());

      std::ofstream out_file(out_path, std::ios::binary);
      if (!out_file) {
        throw std::runtime_error("Failed to create output file: " +
                                 out_path.string());
      }

      in.seekg(static_cast<std::streampos>(fe.offset));

      std::vector<char> encrypted_data(fe.compressed_size);
      in.read(encrypted_data.data(), encrypted_data.size());

      // Check if we read the expected number of bytes
      if (in.gcount() != static_cast<std::streamsize>(fe.compressed_size)) {
        throw std::runtime_error(
            "Failed to read expected number of bytes for file: " +
            out_path.string() + " (expected " +
            std::to_string(fe.compressed_size) + ", got " +
            std::to_string(in.gcount()) + ")");
      }
      if (verbose) {
        std::cout << "Read encrypted data for: " << fe.path << "\n";
      }

      std::vector<char> decrypted_data;
      if (!decrypt(encrypted_data, decrypted_data, fe.encryption_type,
                   password)) {
        throw std::runtime_error("Failed to decrypt file: " +
                                 out_path.string());
      }
      if (verbose) {
        std::cout << "Decrypted file: " << fe.path << " using "
                  << static_cast<int>(fe.encryption_type) << "\n";
      }

      if (decrypted_data.empty()) {
        throw std::runtime_error("Decrypted data is empty for file: " +
                                 out_path.string());
      }

      std::vector<char> decompressed_data;
      if (!decompress(decrypted_data, decompressed_data, fe.size,
                      fe.compression_type)) {
        throw std::runtime_error("Failed to decompress file: " +
                                 out_path.string());
      }
      if (verbose) {
        std::cout << "Decompressed file: " << fe.path << " using "
                  << static_cast<int>(fe.compression_type) << "\n";
      }

      out_file.write(decompressed_data.data(), decompressed_data.size());
      if (verbose) {
        std::cout << "Restored file: " << fe.path << "\n";
      }
    }
  }
}

} // namespace flatpack_archive