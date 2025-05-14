// file_table.hpp
#pragma once

#include "file_entry.hpp"
#include <fstream>
#include <vector>

namespace flatpack_archive {

class FileTable {
public:
  void add_entry(const FileEntry &entry);
  std::vector<FileEntry> &get_entries();
  const size_t count() const;
  void write(std::ofstream &out) const;
  static FileTable read(std::ifstream &in, uint32_t file_count);

private:
  std::vector<FileEntry> entries;
};

} // namespace flatpack_archive