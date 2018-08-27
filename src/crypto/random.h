#pragma once

#include <sodium/randombytes.h>

template <typename T>
inline void random_bytes(gsl::span<T> data)
{
  randombytes_buf(data.data(), data.size_bytes());
}
