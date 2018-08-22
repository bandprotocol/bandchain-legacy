#pragma once

#include <sodium/crypto_hash_sha256.h>

#include "util/bytes.h"

inline Hash sha256(gsl::span<const std::byte> data)
{
  Hash hash;
  crypto_hash_sha256((unsigned char*)hash.data(), (unsigned char*)data.data(),
                     data.size());
  return hash;
}
