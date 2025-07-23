// encryption.cpp

#include "flatpack_archive/encryption.hpp"

namespace flatpack_archive {

bool encrypt(const std::vector<char> &in, std::vector<char> &out,
             EncryptionType encryption_type, const std::string &key) {
  switch (encryption_type) {
  case EncryptionType::None:
    out = in;
    return true;
  case EncryptionType::AES256:
  case EncryptionType::Default:
    return encrypt_aes256(in, out, key);
  default:
    return false;
  }
}

bool decrypt(const std::vector<char> &in, std::vector<char> &out,
             EncryptionType encryption_type, const std::string &key) {
  switch (encryption_type) {
  case EncryptionType::None:
    out = in;
    return true;
  case EncryptionType::AES256:
  case EncryptionType::Default:
    return decrypt_aes256(in, out, key);
  default:
    return false;
  }
}

bool encrypt_aes256(const std::vector<char> &in, std::vector<char> &out,
                    const std::string &password) {
  unsigned char key[AES_KEYLEN] = {};
  unsigned char iv[AES_IVLEN] = {};
  unsigned char salt[PBKDF2_SALTLEN] = {};

  if (!derive_key(password, key, iv, salt))
    return false;

  // Prepare input and output buffers
  std::vector<unsigned char> in_bytes(in.begin(), in.end());
  int in_len = static_cast<int>(in_bytes.size());

  // Output: [salt (16 bytes)] [iv (16 bytes)] [ciphertext]
  out.clear();
  out.reserve(PBKDF2_SALTLEN + AES_IVLEN + in_len +
              AES_IVLEN); // ciphertext may be up to block size longer

  // Write salt and iv to output
  out.insert(out.end(), salt, salt + PBKDF2_SALTLEN);
  out.insert(out.end(), iv, iv + AES_IVLEN);

  // Set up OpenSSL context
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx)
    return false;

  int rc = EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);
  if (rc != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return false;
  }

  std::vector<unsigned char> ciphertext(in_len +
                                        AES_IVLEN); // allow for padding
  int out_len1 = 0;
  int out_len2 = 0;

  rc = EVP_EncryptUpdate(ctx, ciphertext.data(), &out_len1, in_bytes.data(),
                         in_len);
  if (rc != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return false;
  }

  rc = EVP_EncryptFinal_ex(ctx, ciphertext.data() + out_len1, &out_len2);
  if (rc != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return false;
  }

  EVP_CIPHER_CTX_free(ctx);

  out.insert(out.end(), ciphertext.begin(),
             ciphertext.begin() + out_len1 + out_len2);
  return true;
}

bool decrypt_aes256(const std::vector<char> &in, std::vector<char> &out,
                    const std::string &password) {
  if (in.size() <= PBKDF2_SALTLEN + AES_IVLEN)
    return false;

  unsigned char key[AES_KEYLEN] = {};
  unsigned char salt[PBKDF2_SALTLEN] = {};
  unsigned char iv[AES_IVLEN] = {};

  // Input: [salt (16 bytes)] [iv (16 bytes)] [ciphertext]
  // First PBKDF2_SALTLEN bytes for salt and the next AES_IVLEN bytes for iv
  // Copy exact bytes for salt and iv
  std::memcpy(salt, in.data(), PBKDF2_SALTLEN);
  std::memcpy(iv, in.data() + PBKDF2_SALTLEN, AES_IVLEN);

  if (!derive_key(password, key, salt))
    return false;

  // Decrypt the ciphertext portion of 'in' using AES-256-CBC
  size_t ciphertext_offset = PBKDF2_SALTLEN + AES_IVLEN;
  size_t ciphertext_len = in.size() - ciphertext_offset;

  const unsigned char *ciphertext =
      reinterpret_cast<const unsigned char *>(in.data() + ciphertext_offset);

  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx)
    return false;

  int rc = EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);
  if (rc != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return false;
  }

  std::vector<unsigned char> raw_data(ciphertext_len +
                                      AES_IVLEN); // allow for padding
  int out_len1 = 0;
  int out_len2 = 0;

  rc = EVP_DecryptUpdate(ctx, raw_data.data(), &out_len1, ciphertext,
                         ciphertext_len);
  if (rc != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return false;
  }

  rc = EVP_DecryptFinal_ex(ctx, raw_data.data() + out_len1, &out_len2);
  if (rc != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return false;
  }

  EVP_CIPHER_CTX_free(ctx);

  out.insert(out.end(), raw_data.begin(),
             raw_data.begin() + out_len1 + out_len2);

  return true;
}

} // namespace flatpack_archive