// file_entry.hpp
#pragma once

#include <cstdint>
#include <fstream>

namespace flatpack_archive {

struct FileEntry {
  char path[256] = {};
  uint64_t offset = 0;
  uint64_t size = 0;
  uint8_t is_directory = 0;

  void write(std::ofstream &out) const;

  static FileEntry read(std::ifstream &in);
};

} // namespace flatpack_archive