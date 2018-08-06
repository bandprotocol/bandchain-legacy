#include "ed25519.h"

#include <sodium/crypto_sign_ed25519.h>

bool ed25519_verify(const Bytes<64>& sig, const Bytes<32>& verify_key,
                    const unsigned char* data, size_t size)
{
  return crypto_sign_ed25519_verify_detached(sig.data(), data, size,
                                             verify_key.data());
}

bool ed25519_verify(const Bytes<64>& sig, const Bytes<32>& verify_key,
                    const char* data, size_t size)
{
  return ed25519_verify(sig, verify_key, (const unsigned char*)data, size);
}

bool ed25519_verify(const Bytes<64>& sig, const Bytes<32>& verify_key,
                    const std::string& data)
{
  return ed25519_verify(sig, verify_key, data.c_str(), data.size());
}
