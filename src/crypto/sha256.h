#pragma once

#include "util/bytes.h"

/// Compute SHA-256 hash of a given data buffer and size.
Bytes<32> sha256(const unsigned char* data, size_t size);
Bytes<32> sha256(const char* data, size_t size);

/// Compute SHA-256 hash of a given structure.
Bytes<32> sha256(const std::string& data);
