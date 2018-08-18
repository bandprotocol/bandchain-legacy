#include "sha256.h"

#include <sodium/crypto_hash_sha256.h>

Hash sha256(const void* data, size_t size)
{
  Hash hash;
  crypto_hash_sha256((unsigned char*)hash.data(), (const unsigned char*)data,
                     size);
  return hash;
}

Hash sha256(const std::string& data)
{
  return sha256(data.c_str(), data.size());
}
