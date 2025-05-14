// archive.cpp

#include "flatpack_archive/archive.hpp"
#include "flatpack_archive/file_table.hpp"
#include "flatpack_archive/header.hpp"

#include <filesystem>
#include <fstream>

namespace flatpack_archive {

namespace fs = std::filesystem;

void create_archive(const std::string &input_folder,
                    const std::string &output_file) {
  std::ofstream out(output_file, std::ios::binary);
  if (!out) {
    throw new std::runtime_error("Failed to open archive file for writing");
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
      out << in.rdbuf();
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
                     const std::string &output_folder) {
  std::ifstream in(archive_file, std::ios::binary);
  if (!in) {
    throw new std::runtime_error("Failed to open archive file for reading");
  }

  ArchiveHeader header = ArchiveHeader::read(in);
  if (!header.is_valid()) {
    throw new std::runtime_error("Invalid archive file format");
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
        throw new std::runtime_error("Failed to create output file: " +
                                     out_path.string());
      }

      in.seekg(static_cast<std::streampos>(fe.offset));
      std::vector<char> buffer(fe.size);
      in.read(buffer.data(), buffer.size());
      out_file.write(buffer.data(), buffer.size());
    }
  }
}

} // namespace flatpack_archive