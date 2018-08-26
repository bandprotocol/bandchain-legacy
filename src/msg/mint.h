#pragma once

#include "state/object.h"

class MintMsg : public ObjectBase<MintMsg, ObjectID::Mint>
{
public:
  Address addr;
  BigInt value;
  Hash ident;

  friend Buffer& operator<<=(Buffer& buf, const MintMsg& mint)
  {
    return buf << mint.addr << mint.value << mint.ident;
  }

  friend Buffer& operator>>=(Buffer& buf, MintMsg& mint)
  {
    return buf >> mint.addr >> mint.value >> mint.ident;
  }
private:
  Hash hash() const final { return Hash(); }
};
//static_assert(sizeof(MintMsg) == 84, "Invalid MingMsg size");

template <>
std::unique_ptr<Object> Object::deserialize<MintMsg::ID>(Buffer& buf)
{
  return MintMsg::deserialize_impl(buf);
}
