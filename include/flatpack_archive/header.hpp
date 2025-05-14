// header.hpp
#pragma once

#include <cstdint>
#include <cstring>
#include <fstream>

namespace flatpack_archive {

inline constexpr const char MAGIC[] = "FPACK";
inline constexpr uint8_t MAJOR_VERSION = 1;
inline constexpr uint8_t MINOR_VERSION = 0;
inline constexpr uint8_t PATCH_VERSION = 0;

struct ArchiveHeader {
  char magic[6] = {};
  uint8_t major_version = MAJOR_VERSION;
  uint8_t minor_version = MINOR_VERSION;
  uint8_t patch_version = PATCH_VERSION;
  uint32_t file_count = 0;

  static constexpr size_t HEADER_SIZE =
      sizeof(magic) + sizeof(major_version) + sizeof(minor_version) +
      sizeof(patch_version) + sizeof(file_count);

  ArchiveHeader() { std::memcpy(magic, MAGIC, sizeof(magic)); }

  void write(std::ofstream &out) const;

  static ArchiveHeader read(std::ifstream &in);

  bool is_valid() const {
    return std::strncmp(magic, MAGIC, 5) == 0 &&
           major_version == MAJOR_VERSION && minor_version == MINOR_VERSION &&
           patch_version == PATCH_VERSION;
  }
};

} // namespace flatpack_archive