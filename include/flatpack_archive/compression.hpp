// compression.hpp
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace flatpack_archive {

constexpr int ZSTD_COMPRESSION_LEVEL = 1;

enum class CompressionType : uint8_t {
  None = 0,
  Zlib = 1,
  LZ4 = 2,
  Zstd = 3,
  Brotli = 4,
  Default = 5
};

bool compress(const std::vector<char> &in, std::vector<char> &out,
              CompressionType type);
bool decompress(const std::vector<char> &in, std::vector<char> &out,
                size_t uncompressed_size, CompressionType type);

bool compress_zlib(const std::vector<char> &in, std::vector<char> &out);
bool decompress_zlib(const std::vector<char> &in, std::vector<char> &out,
                     size_t uncompressed_size);

bool compress_lz4(const std::vector<char> &in, std::vector<char> &out);
bool decompress_lz4(const std::vector<char> &in, std::vector<char> &out,
                    size_t uncompressed_size);

bool compress_zstd(const std::vector<char> &in, std::vector<char> &out);
bool decompress_zstd(const std::vector<char> &in, std::vector<char> &out,
                     size_t uncompressed_size);

bool compress_brotli(const std::vector<char> &in, std::vector<char> &out);
bool decompress_brotli(const std::vector<char> &in, std::vector<char> &out,
                       size_t uncompressed_size);

inline CompressionType parse_compression_type(const std::string &type) {
  if (type == "zlib")
    return CompressionType::Zlib;
  if (type == "lz4")
    return CompressionType::LZ4;
  if (type == "zstd")
    return CompressionType::Zstd;
  if (type == "brotli")
    return CompressionType::Brotli;
  return CompressionType::Default;
}

} // namespace flatpack_archive