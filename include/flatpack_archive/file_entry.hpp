// file_entry.hpp
#pragma once

#include "compression.hpp"
#include "flatpack_archive/encryption.hpp"

#include <cstdint>
#include <fstream>

namespace flatpack_archive {

struct FileEntry {
  char path[256] = {};
  uint64_t offset = 0;
  uint64_t size = 0;
  uint64_t compressed_size = 0;
  uint8_t is_directory = 0;
  CompressionType compression_type = CompressionType::None;
  EncryptionType encryption_type = EncryptionType::None;

  void write(std::ofstream &out) const;

  static FileEntry read(std::ifstream &in);
};

} // namespace flatpack_archive