#pragma once

#include "util/bytes.h"

/// Verify Ed25519 signature.
bool ed25519_verify(const Bytes<64>& sig, const Bytes<32>& verify_key,
                    const unsigned char* data, size_t size);
bool ed25519_verify(const Bytes<64>& sig, const Bytes<32>& verify_key,
                    const char* data, size_t size);

/// Verify variants that do not take size.
bool ed25519_verify(const Bytes<64>& sig, const Bytes<32>& verify_key,
                    const std::string& data);
