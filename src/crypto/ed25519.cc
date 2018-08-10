#include "ed25519.h"

#include <sodium/crypto_sign_ed25519.h>

std::pair<VerifyKey, SecretKey> ed25519_generate_keypair()
{
  std::pair<VerifyKey, SecretKey> keypair;
  crypto_sign_ed25519_keypair((unsigned char*)keypair.first.data(),
                              (unsigned char*)keypair.second.data());
  return keypair;
}

VerifyKey ed25519_sk_to_vk(const SecretKey& secret_key)
{
  VerifyKey verify_key;
  crypto_sign_ed25519_sk_to_pk((unsigned char*)verify_key.data(),
                               (const unsigned char*)secret_key.data());
  return verify_key;
}
int crypto_sign_ed25519_sk_to_pk(unsigned char* pk, const unsigned char* sk);

Signature ed25519_sign(const SecretKey& secret_key, const unsigned char* data,
                       size_t size)
{
  SecretKey sig;
  crypto_sign_ed25519_detached((unsigned char*)sig.data(), nullptr, data, size,
                               (const unsigned char*)secret_key.data());
  return sig;
}

Signature ed25519_sign(const SecretKey& secret_key, const char* data,
                       size_t size)
{
  return ed25519_sign(secret_key, (const unsigned char*)data, size);
}

Signature ed25519_sign(const SecretKey& secret_key, std::byte* data,
                       size_t size)
{
  return ed25519_sign(secret_key, (const unsigned char*)data, size);
}

Signature ed25519_sign(const SecretKey& secret_key, const std::string& data)
{
  return ed25519_sign(secret_key, data.c_str(), data.size());
}

bool ed25519_verify(const Signature& sig, const VerifyKey& verify_key,
                    const unsigned char* data, size_t size)
{
  return crypto_sign_ed25519_verify_detached(
             (const unsigned char*)sig.data(), data, size,
             (const unsigned char*)verify_key.data()) == 0;
}

bool ed25519_verify(const Signature& sig, const VerifyKey& verify_key,
                    const char* data, size_t size)
{
  return ed25519_verify(sig, verify_key, (const unsigned char*)data, size);
}

bool ed25519_verify(const Signature& sig, const VerifyKey& verify_key,
                    const std::byte* data, size_t size)
{
  return ed25519_verify(sig, verify_key, (const unsigned char*)data, size);
}

bool ed25519_verify(const Signature& sig, const VerifyKey& verify_key,
                    const std::string& data)
{
  return ed25519_verify(sig, verify_key, data.c_str(), data.size());
}
