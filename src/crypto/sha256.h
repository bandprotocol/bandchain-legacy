#pragma once

#include "util/bytes.h"

/// Compute SHA-256 hash of a given data buffer and size.
Hash sha256(const void* data, size_t size);
Hash sha256(const std::string& data);

template <int SIZE>
Hash sha256(const Bytes<SIZE>& data)
{
  return sha256(data.data(), SIZE);
}
