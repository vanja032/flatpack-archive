// archive.hpp
#pragma once

#include "flatpack_archive/compression.hpp"
#include "flatpack_archive/encryption.hpp"

#include <string>

namespace flatpack_archive {

void create_archive(const std::string &input_folder,
                    const std::string &output_file,
                    CompressionType compression_type,
                    EncryptionType encryption_type, const std::string &password,
                    const bool verbose);

void extract_archive(const std::string &archive_file,
                     const std::string &output_folder,
                     const std::string &password, const bool verbose);

} // namespace flatpack_archive
