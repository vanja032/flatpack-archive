// test.cpp

#include "../include/flatpack_archive/archive.hpp"
#include "../include/flatpack_archive/compression.hpp"

#define MOD 0
int main() {
  if (MOD == 1) {
    flatpack_archive::create_archive(
        "../resources/test", "../resources/test.flatpack",
        flatpack_archive::CompressionType::Default);
  } else {
    flatpack_archive::extract_archive("../resources/test.flatpack",
                                      "../resources/test");
  }

  return 0;
}