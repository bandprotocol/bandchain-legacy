#include "msg/tx.h"

size_t TxMsg::size() const
{
  return sizeof(TxMsg) + input_cnt * sizeof(TxMsg::TxInput) +
         output_cnt * sizeof(TxMsg::TxOutput) + input_cnt * sizeof(Signature);
}

Hash TxMsg::hash() const
{
  size_t size_to_hash = sizeof(TxMsg) + input_cnt * sizeof(TxMsg::TxInput) +
                        output_cnt * sizeof(TxMsg::TxOutput);
  return sha256(reinterpret_cast<const std::byte*>(this), size_to_hash);
}

TxMsg::TxInput& TxMsg::get_input(uint8_t idx)
{
  std::byte* ext = reinterpret_cast<std::byte*>(this) + sizeof(TxMsg);
  size_t offset = idx * sizeof(TxMsg::TxInput);
  return *reinterpret_cast<TxMsg::TxInput*>(ext + offset);
}

const TxMsg::TxInput& TxMsg::get_input(uint8_t idx) const
{
  const std::byte* ext =
      reinterpret_cast<const std::byte*>(this) + sizeof(TxMsg);
  size_t offset = idx * sizeof(TxMsg::TxInput);
  return *reinterpret_cast<const TxMsg::TxInput*>(ext + offset);
}

TxMsg::TxOutput& TxMsg::get_output(uint8_t idx)
{
  std::byte* ext = reinterpret_cast<std::byte*>(this) + sizeof(TxMsg);
  size_t offset =
      input_cnt * sizeof(TxMsg::TxInput) + idx * sizeof(TxMsg::TxOutput);
  return *reinterpret_cast<TxOutput*>(ext + offset);
}

const TxMsg::TxOutput& TxMsg::get_output(uint8_t idx) const
{
  const std::byte* ext =
      reinterpret_cast<const std::byte*>(this) + sizeof(TxMsg);
  size_t offset =
      input_cnt * sizeof(TxMsg::TxInput) + idx * sizeof(TxMsg::TxOutput);
  return *reinterpret_cast<const TxMsg::TxOutput*>(ext + offset);
}

Signature& TxMsg::get_signature(uint8_t idx)
{
  std::byte* ext = reinterpret_cast<std::byte*>(this) + sizeof(TxMsg);
  size_t offset = input_cnt * sizeof(TxMsg::TxInput) +
                  output_cnt * sizeof(TxMsg::TxOutput) +
                  idx * sizeof(Signature);
  return *reinterpret_cast<Signature*>(ext + offset);
}

const Signature& TxMsg::get_signature(uint8_t idx) const
{
  const std::byte* ext =
      reinterpret_cast<const std::byte*>(this) + sizeof(TxMsg);
  size_t offset = input_cnt * sizeof(TxMsg::TxInput) +
                  output_cnt * sizeof(TxMsg::TxOutput) +
                  idx * sizeof(Signature);
  return *reinterpret_cast<const Signature*>(ext + offset);
}
