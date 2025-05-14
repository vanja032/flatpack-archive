// file_entry.cpp

#include "flatpack_archive/file_entry.hpp"

namespace flatpack_archive {

void FileEntry::write(std::ofstream &out) const {
  out.write(reinterpret_cast<const char *>(path), sizeof(path));
  out.write(reinterpret_cast<const char *>(&offset), sizeof(offset));
  out.write(reinterpret_cast<const char *>(&size), sizeof(size));
  out.write(reinterpret_cast<const char *>(&is_directory),
            sizeof(is_directory));
}

FileEntry FileEntry::read(std::ifstream &in) {
  FileEntry entry;
  in.read(reinterpret_cast<char *>(entry.path), sizeof(entry.path));
  in.read(reinterpret_cast<char *>(&entry.offset), sizeof(entry.offset));
  in.read(reinterpret_cast<char *>(&entry.size), sizeof(entry.size));
  in.read(reinterpret_cast<char *>(&entry.is_directory),
          sizeof(entry.is_directory));

  return entry;
}

} // namespace flatpack_archive