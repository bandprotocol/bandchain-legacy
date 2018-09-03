#pragma once

#include <sodium/randombytes.h>

#include "inc/essential.h"

template <typename T>
inline void random_bytes(gsl::span<T> data)
{
  randombytes_buf(data.data(), data.size_bytes());
}
