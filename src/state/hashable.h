#pragma once

#include "util/bytes.h"

enum class HashableType : uint8_t {
  Nil = 0,
  TxOutput = 0,
};

class Hashable
{
public:
  virtual ~Hashable(){};

  /// Return the type of this hashable.
  virtual HashableType type() const = 0;

  /// Return the key. Key must be unique per hashable structure and should
  /// not change as the structure mutates.
  virtual Hash key() const = 0;

  /// Return the hash value of this structure.
  virtual Hash hash() const = 0;

  /// Return the hashable's string representation.
  virtual std::string to_string() const = 0;
};

class HashableNil : public Hashable
{
  /// As required by Hashable.
  HashableType type() const final { return HashableType::Nil; }
  Hash key() const final { return Hash(); }
  Hash hash() const final { return Hash(); }
  std::string to_string() const { return "NIL"; }
};
