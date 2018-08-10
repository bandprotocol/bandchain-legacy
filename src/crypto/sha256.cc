#include "sha256.h"

#include <sodium/crypto_hash_sha256.h>

Bytes<32> sha256(const unsigned char* data, size_t size)
{
  Bytes<32> hash;
  crypto_hash_sha256((unsigned char*)hash.data(), data, size);
  return hash;
}

Bytes<32> sha256(const char* data, size_t size)
{
  return sha256((const unsigned char*)data, size);
}

Bytes<32> sha256(const std::byte* data, size_t size)
{
  return sha256((const unsigned char*)data, size);
}

Bytes<32> sha256(const std::string& data)
{
  return sha256(data.c_str(), data.size());
}
