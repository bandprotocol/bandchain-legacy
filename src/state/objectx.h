#pragma once

#include "util/bytes.h"

class Object
{
public:
  virtual ~Object(){};

  /// Return the key. Key must be unique per object and should not change as
  /// the object mutates.
  virtual Hash key() const = 0;

  /// Return the hash value of this object.
  virtual Hash hash() const = 0;

  /// Return the object's string representation.
  virtual std::string to_string() const = 0;
};
