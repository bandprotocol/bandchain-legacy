#include "ed25519.h"

#include <sodium/crypto_sign_ed25519.h>

#include "crypto/sha256.h"

std::pair<VerifyKey, SecretKey> ed25519_generate_keypair()
{
  std::pair<VerifyKey, SecretKey> keypair;
  crypto_sign_ed25519_keypair(keypair.first.data(), keypair.second.data());
  return keypair;
}

VerifyKey ed25519_sk_to_vk(const SecretKey& secret_key)
{
  VerifyKey verify_key;
  crypto_sign_ed25519_sk_to_pk(verify_key.data(), secret_key.data());
  return verify_key;
}

Address ed25519_vk_to_addr(const VerifyKey& verify_key)
{
  return sha256(verify_key.data(), VerifyKey::Size).suffix<Address::Size>();
}

Signature ed25519_sign(const SecretKey& secret_key, const void* data,
                       size_t size)
{
  SecretKey sig;
  crypto_sign_ed25519_detached(sig.data(), nullptr, (const unsigned char*)data,
                               size, secret_key.data());
  return sig;
}

Signature ed25519_sign(const SecretKey& secret_key, const std::string& data)
{
  return ed25519_sign(secret_key, data.c_str(), data.size());
}

bool ed25519_verify(const Signature& sig, const VerifyKey& verify_key,
                    const void* data, size_t size)
{
  return crypto_sign_ed25519_verify_detached(sig.data(),
                                             (const unsigned char*)data, size,
                                             verify_key.data()) == 0;
}

bool ed25519_verify(const Signature& sig, const VerifyKey& verify_key,
                    const std::string& data)
{
  return ed25519_verify(sig, verify_key, data.c_str(), data.size());
}
