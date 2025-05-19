// archive.hpp
#pragma once

#include "compression.hpp"

#include <string>

namespace flatpack_archive {

void create_archive(const std::string &input_folder,
                    const std::string &output_file,
                    CompressionType compression_type);

void extract_archive(const std::string &archive_file,
                     const std::string &output_folder);

} // namespace flatpack_archive
