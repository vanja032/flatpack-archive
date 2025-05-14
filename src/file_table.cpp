// file_table.cpp

#include "flatpack_archive/file_table.hpp"

namespace flatpack_archive {

void FileTable::add_entry(const FileEntry &entry) { entries.push_back(entry); }

std::vector<FileEntry> &FileTable::get_entries() { return entries; }

const size_t FileTable::count() const { return entries.size(); }

void FileTable::write(std::ofstream &out) const {
  for (const auto &entry : entries) {
    entry.write(out);
  }
}

FileTable FileTable::read(std::ifstream &in, uint32_t file_count) {
  FileTable table;
  for (uint32_t i = 0; i < file_count; ++i) {
    table.entries.push_back(FileEntry::read(in));
  }
  return table;
}

} // namespace flatpack_archive