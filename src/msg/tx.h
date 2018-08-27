#pragma once

#include "crypto/ed25519.h"
#include "crypto/sha256.h"
#include "state/object.h"

class TxMsg : public ObjectBase<TxMsg, ObjectID::Tx>
{
public:
  VerifyKey vk;
  uint256_t value;
  uint256_t nonce;
  Address addr;
  Signature sig;

  friend Buffer& operator<<=(Buffer& buf, const TxMsg& tx)
  {
    return buf << tx.vk << tx.value << tx.nonce << tx.addr << tx.sig;
  }

  friend Buffer& operator>>=(Buffer& buf, TxMsg& tx)
  {
    return buf >> tx.vk >> tx.value >> tx.nonce >> tx.addr >> tx.sig;
  }

  bool verify();

private:
  Hash hash() const final { return Hash(); }
};

template <>
std::unique_ptr<Object> Object::deserialize<TxMsg::ID>(Buffer& buf)
{
  return TxMsg::deserialize_impl(buf);
}

inline bool TxMsg::verify()
{
  Buffer buf_temp;
  buf_temp << vk << value << nonce << addr;
  return ed25519_verify(sig, vk,
                        gsl::span(buf_temp.begin(), buf_temp.size_bytes()));
}
