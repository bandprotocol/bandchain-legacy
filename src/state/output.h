#pragma once

#include "state/objectx.h"

class TxOutput : public Object
{
public:
  TxOutput(const Address& _owner, const uint256_t& _value, const Hash& _ident);
  TxOutput(const TxOutput& rhs) = default;

  Hash key() const final;
  Hash hash() const final;
  std::string to_string() const final;

  /// Spend this transaction output.
  void spend() { spent = true; }

  /// Check if this transaction output can be spent by the given verify key.
  bool is_spendable(const VerifyKey& vk) const;

  /// Getter method to return the total value of this transaction ouput.
  uint256_t get_value() const { return value; }

private:
  Address owner;      //< The address of the transaction output's owner
  uint256_t value;    //< The total value of this transaction output
  Hash ident;         //< Unique identifier of this transaction output
  bool spent = false; //< Whether this output is spent
};
