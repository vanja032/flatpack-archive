// encryption.hpp
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <openssl/evp.h>
#include <openssl/rand.h>

namespace flatpack_archive {

inline constexpr int AES_KEYLEN = 32;                         // 256 bits
inline constexpr int AES_IVLEN = 16;                          // 128 bits
inline constexpr int PBKDF2_ITER = 1000;                      // 1000 iterations
inline constexpr int PBKDF2_SALTLEN = 16;                     // 128 bits
inline constexpr const unsigned char PBKDF2_SALT[] = "fpack"; // 5 bytes

enum class EncryptionType : uint8_t { None = 0, AES256 = 1, Default = 2 };

bool encrypt(const std::vector<char> &in, std::vector<char> &out,
             EncryptionType type, const std::string &password);
bool decrypt(const std::vector<char> &in, std::vector<char> &out,
             EncryptionType type, const std::string &password);

bool encrypt_aes256(const std::vector<char> &in, std::vector<char> &out,
                    const std::string &password);
bool decrypt_aes256(const std::vector<char> &in, std::vector<char> &out,
                    const std::string &password);

inline EncryptionType parse_encryption_type(const std::string &type) {
  if (type == "aes256")
    return EncryptionType::AES256;
  return EncryptionType::Default;
}

// Helper: Derive key from password using PBKDF2
inline bool derive_key(const std::string &password, unsigned char *key,
                       const unsigned char *salt) {
  return PKCS5_PBKDF2_HMAC(password.c_str(), password.size(), salt,
                           PBKDF2_SALTLEN, PBKDF2_ITER, EVP_sha256(),
                           AES_KEYLEN, key) == 1;
}

// Helper: Derive key from password using PBKDF2, Generate random IV and salt
inline bool derive_key(const std::string &password, unsigned char *key,
                       unsigned char *iv, unsigned char *salt) {
  std::memcpy(salt, PBKDF2_SALT, sizeof(PBKDF2_SALT));
  if (RAND_bytes(salt + sizeof(PBKDF2_SALT),
                 PBKDF2_SALTLEN - sizeof(PBKDF2_SALT)) != 1)
    return false;

  if (RAND_bytes(iv, AES_IVLEN) != 1)
    return false;

  return PKCS5_PBKDF2_HMAC(password.c_str(), password.size(), salt,
                           PBKDF2_SALTLEN, PBKDF2_ITER, EVP_sha256(),
                           AES_KEYLEN, key) == 1;
}

} // namespace flatpack_archive