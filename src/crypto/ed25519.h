#pragma once

#include <sodium/crypto_sign_ed25519.h>

#include "crypto/sha256.h"
#include "util/bytes.h"

/// Generate a random Ed25519 keypair
inline std::pair<VerifyKey, SecretKey> ed25519_generate_keypair()
{
  std::pair<VerifyKey, SecretKey> keypair;
  crypto_sign_ed25519_keypair(keypair.first.data(), keypair.second.data());
  return keypair;
}

/// Convert Ed25519 secret key to Ed25519 verify key
inline VerifyKey ed25519_sk_to_vk(const SecretKey& secret_key)
{
  VerifyKey verify_key;
  crypto_sign_ed25519_sk_to_pk(verify_key.data(), secret_key.data());
  return verify_key;
}

/// Convert Ed25519 verify key to address (unique to BAND).
inline Address ed25519_vk_to_addr(const VerifyKey& verify_key)
{
  return sha256(verify_key).suffix<Address::Size>();
}

/// Sign Ed25519 signature.
inline Signature ed25519_sign(const SecretKey& secret_key,
                              gsl::span<const unsigned char> data)
{
  SecretKey sig;
  crypto_sign_ed25519_detached(sig.data(), nullptr, data.data(), data.size(),
                               secret_key.data());
  return sig;
}

/// Verify Ed25519 signature.
inline bool ed25519_verify(const Signature& sig, const VerifyKey& verify_key,
                           gsl::span<const unsigned char> data)
{
  return crypto_sign_ed25519_verify_detached(
             sig.data(), data.data(), data.size(), verify_key.data()) == 0;
}
