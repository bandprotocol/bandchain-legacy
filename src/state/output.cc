#include "output.h"

#include "crypto/ed25519.h"
#include "crypto/sha256.h"

TxOutput::TxOutput(const Address& _owner, const uint256_t& _value,
                   const Hash& _ident)
    : owner(_owner)
    , value(_value)
    , ident(_ident)
{
}

Hash TxOutput::key() const { return ident; }

Hash TxOutput::hash() const
{
  return spent ? sha256(owner + BigInt::from_uint256(value) + ident) : Hash();
}

std::string TxOutput::to_string() const
{
  return "TxOutput@{}(owner={} value={}{})"_format(ident, owner, value,
                                                   spent ? " [SPENT]" : "");
}

bool TxOutput::spendable(const VerifyKey& vk) const
{
  return !spent && ed25519_vk_to_addr(vk) == owner;
}
