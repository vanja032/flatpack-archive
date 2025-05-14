// header.cpp

#include "flatpack_archive/header.hpp"

namespace flatpack_archive {

void ArchiveHeader::write(std::ofstream &out) const {
  out.write(reinterpret_cast<const char *>(magic), sizeof(magic));
  out.write(reinterpret_cast<const char *>(&major_version),
            sizeof(major_version));
  out.write(reinterpret_cast<const char *>(&minor_version),
            sizeof(minor_version));
  out.write(reinterpret_cast<const char *>(&patch_version),
            sizeof(patch_version));
  out.write(reinterpret_cast<const char *>(&file_count), sizeof(file_count));
}

ArchiveHeader ArchiveHeader::read(std::ifstream &in) {
  ArchiveHeader header;
  in.read(reinterpret_cast<char *>(header.magic), sizeof(header.magic));
  in.read(reinterpret_cast<char *>(&header.major_version),
          sizeof(header.major_version));
  in.read(reinterpret_cast<char *>(&header.minor_version),
          sizeof(header.minor_version));
  in.read(reinterpret_cast<char *>(&header.patch_version),
          sizeof(header.patch_version));
  in.read(reinterpret_cast<char *>(&header.file_count),
          sizeof(header.file_count));

  return header;
}

} // namespace flatpack_archive