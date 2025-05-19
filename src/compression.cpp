// compression.cpp

#include "flatpack_archive/compression.hpp"

#include <brotli/decode.h>
#include <brotli/encode.h>
#include <lz4.h>
#include <vector>
#include <zlib.h>
#include <zstd.h>

namespace flatpack_archive {

bool compress(const std::vector<char> &in, std::vector<char> &out,
              CompressionType type) {
  switch (type) {
  case CompressionType::None:
    out = in;
    return true;
  case CompressionType::Zlib:
  case CompressionType::Default:
    return compress_zlib(in, out);
  case CompressionType::LZ4:
    return compress_lz4(in, out);
  case CompressionType::Zstd:
    return compress_zstd(in, out);
  case CompressionType::Brotli:
    return compress_brotli(in, out);
  default:
    return false;
  }
}

bool decompress(const std::vector<char> &in, std::vector<char> &out,
                size_t uncompressed_size, CompressionType type) {
  switch (type) {
  case CompressionType::None:
    out = in;
    return true;
  case CompressionType::Zlib:
  case CompressionType::Default:
    return decompress_zlib(in, out, uncompressed_size);
  case CompressionType::LZ4:
    return decompress_lz4(in, out, uncompressed_size);
  case CompressionType::Zstd:
    return decompress_zstd(in, out, uncompressed_size);
  case CompressionType::Brotli:
    return decompress_brotli(in, out, uncompressed_size);
  default:
    return false;
  }
}

bool compress_zlib(const std::vector<char> &in, std::vector<char> &out) {
  uLongf data_size = compressBound(in.size());
  out.resize(data_size);
  int result =
      ::compress(reinterpret_cast<Bytef *>(out.data()), &data_size,
                 reinterpret_cast<const Bytef *>(in.data()), in.size());

  if (result != Z_OK) {
    return false;
  }

  out.resize(data_size);
  return true;
}

bool decompress_zlib(const std::vector<char> &in, std::vector<char> &out,
                     size_t uncompressed_size) {
  out.resize(uncompressed_size);
  uLongf data_size = static_cast<uLongf>(uncompressed_size);
  int result =
      uncompress(reinterpret_cast<Bytef *>(out.data()), &data_size,
                 reinterpret_cast<const Bytef *>(in.data()), in.size());
  out.resize(data_size);
  return result == Z_OK;
}

bool compress_lz4(const std::vector<char> &in, std::vector<char> &out) {
  int data_size = LZ4_compressBound(static_cast<int>(in.size()));
  out.resize(data_size);
  int compressed_size = LZ4_compress_default(
      in.data(), out.data(), static_cast<int>(in.size()), data_size);
  if (compressed_size <= 0)
    return false;
  out.resize(compressed_size);
  return true;
}

bool decompress_lz4(const std::vector<char> &in, std::vector<char> &out,
                    size_t uncompressed_size) {
  out.resize(uncompressed_size);
  int result = LZ4_decompress_safe(in.data(), out.data(), in.size(),
                                   static_cast<int>(uncompressed_size));
  bool ok = result >= 0;
  if (ok)
    out.resize(result);
  return ok;
}

bool compress_zstd(const std::vector<char> &in, std::vector<char> &out) {
  size_t data_size = ZSTD_compressBound(in.size());
  out.resize(data_size);
  size_t compressed_size = ZSTD_compress(out.data(), data_size, in.data(),
                                         in.size(), ZSTD_COMPRESSION_LEVEL);
  if (ZSTD_isError(compressed_size))
    return false;
  out.resize(compressed_size);
  return true;
}

bool decompress_zstd(const std::vector<char> &in, std::vector<char> &out,
                     size_t uncompressed_size) {
  out.resize(uncompressed_size);
  size_t result =
      ZSTD_decompress(out.data(), uncompressed_size, in.data(), in.size());

  bool ok = !ZSTD_isError(result);
  if (ok)
    out.resize(result);
  return ok;
}

bool compress_brotli(const std::vector<char> &in, std::vector<char> &out) {
  size_t data_size = BrotliEncoderMaxCompressedSize(in.size());
  out.resize(data_size);
  if (!BrotliEncoderCompress(
          BROTLI_DEFAULT_QUALITY, BROTLI_DEFAULT_WINDOW, BROTLI_MODE_GENERIC,
          in.size(), reinterpret_cast<const uint8_t *>(in.data()), &data_size,
          reinterpret_cast<uint8_t *>(out.data()))) {
    return false;
  }
  out.resize(data_size);
  return true;
}

bool decompress_brotli(const std::vector<char> &in, std::vector<char> &out,
                       size_t uncompressed_size) {
  out.resize(uncompressed_size);
  size_t data_size = uncompressed_size;
  if (BrotliDecoderDecompress(
          in.size(), reinterpret_cast<const uint8_t *>(in.data()), &data_size,
          reinterpret_cast<uint8_t *>(out.data())) !=
      BROTLI_DECODER_RESULT_SUCCESS) {
    return false;
  }
  out.resize(data_size);
  return true;
}

} // namespace flatpack_archive