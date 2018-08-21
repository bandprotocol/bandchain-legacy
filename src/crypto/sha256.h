#pragma once

#include <sodium/crypto_hash_sha256.h>

#include "util/bytes.h"

inline Hash sha256(gsl::span<const unsigned char> data)
{
  Hash hash;
  crypto_hash_sha256(hash.data(), data.data(), data.size());
  return hash;
}
