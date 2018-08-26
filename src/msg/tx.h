#pragma once

#include "crypto/sha256.h"
#include "crypto/ed25519.h"
#include "state/object.h"

class TxMsg : public ObjectBase<TxMsg, ObjectID::Tx>
{
public:
    VerifyKey vk;
    BigInt value;
    BigInt nonce;
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
  return ed25519_verify(sig, vk, gsl::span(buf_temp.begin(), buf_temp.size_bytes()));
}

// struct TxMsg : public MsgBase<TxMsg, MsgID::Tx> {
//   big_uint8_t input_count = 0;
//   big_uint8_t output_count = 0;

//   struct TxInput {
//     Hash ident;
//     VerifyKey vk;
//     Signature sig;
//   };

//   struct TxOutput {
//     Address addr;
//     nonBigInt value;
//   };

//   static size_t TxSize(uint8_t input_count, uint8_t output_count)
//   {
//     const size_t base_size = sizeof(TxMsg);
//     const size_t input_size = sizeof(TxMsg::TxInput) * input_count;
//     const size_t output_size = sizeof(TxMsg::TxOutput) * output_count;
//     return base_size + input_size + output_size;
//   }

//   size_t msg_size() const { return TxSize(input_count, output_count); }

//   gsl::span<TxInput> inputs();
//   gsl::span<const TxInput> inputs() const;

//   gsl::span<TxOutput> outputs();
//   gsl::span<const TxOutput> outputs() const;
// };
// static_assert(sizeof(TxMsg) == sizeof(MsgBaseVoid) + 2, "Invalid TxMsg
// size");

// ////////////////////////////////////////////////////////////////////////////////
// inline gsl::span<TxMsg::TxInput> TxMsg::inputs()
// {
//   auto base = reinterpret_cast<std::byte*>(this) + sizeof(TxMsg);
//   return {reinterpret_cast<TxInput*>(base), input_count};
// }

// inline gsl::span<const TxMsg::TxInput> TxMsg::inputs() const
// {
//   auto base = reinterpret_cast<const std::byte*>(this) + sizeof(TxMsg);
//   return {reinterpret_cast<const TxInput*>(base), input_count};
// }

// inline gsl::span<TxMsg::TxOutput> TxMsg::outputs()
// {
//   auto base = reinterpret_cast<std::byte*>(this) + sizeof(TxMsg);
//   auto offset = sizeof(TxMsg::TxInput) * input_count;
//   return {reinterpret_cast<TxOutput*>(base + offset), output_count};
// }

// inline gsl::span<const TxMsg::TxOutput> TxMsg::outputs() const
// {
//   auto base = reinterpret_cast<const std::byte*>(this) + sizeof(TxMsg);
//   auto offset = sizeof(TxMsg::TxInput) * input_count;
//   return {reinterpret_cast<const TxOutput*>(base + offset), output_count};
// }
