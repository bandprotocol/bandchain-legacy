#pragma once

#include "util/bytes.h"

/// Generate a random Ed25519 keypair
std::pair<VerifyKey, SecretKey> ed25519_generate_keypair();

/// Convert Ed25519 secret key to Ed25519 verify key
VerifyKey ed25519_sk_to_vk(const SecretKey& secret_key);

/// Convert Ed25519 verify key to address (unique to BAND).
Address ed25519_vk_to_addr(const VerifyKey& verify_key);

/// Sign Ed25519 signature.
Signature ed25519_sign(const SecretKey& secret_key, const unsigned char* data,
                       size_t size);
Signature ed25519_sign(const SecretKey& secret_key, const char* data,
                       size_t size);
Signature ed25519_sign(const SecretKey& secret_key, const std::byte* data,
                       size_t size);
Signature ed25519_sign(const SecretKey& secret_key, const std::string& data);

/// Verify Ed25519 signature.
bool ed25519_verify(const Signature& sig, const VerifyKey& verify_key,
                    const unsigned char* data, size_t size);
bool ed25519_verify(const Signature& sig, const VerifyKey& verify_key,
                    const char* data, size_t size);
bool ed25519_verify(const Signature& sig, const VerifyKey& verify_key,
                    const std::byte* data, size_t size);
bool ed25519_verify(const Signature& sig, const VerifyKey& verify_key,
                    const std::string& data);
