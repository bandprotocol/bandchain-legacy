#include "sha256.h"

#include <sodium/crypto_hash_sha256.h>

Hash sha256(const unsigned char* data, size_t size)
{
  Hash hash;
  crypto_hash_sha256((unsigned char*)hash.data(), data, size);
  return hash;
}

Hash sha256(const char* data, size_t size)
{
  return sha256((const unsigned char*)data, size);
}

Hash sha256(const std::byte* data, size_t size)
{
  return sha256((const unsigned char*)data, size);
}

Hash sha256(const std::string& data)
{
  return sha256(data.c_str(), data.size());
}
