#pragma once

#include "inc/essential.h"

template <typename T>
struct TypeID {
  static const char* name;
};

#define TYPEID(type)                                                           \
  template <>                                                                  \
  inline const char* TypeID<type>::name = #type;

#define TYPEID_CUSTOM(type, custom_name)                                       \
  template <>                                                                  \
  inline const char* TypeID<type>::name = #custom_name;

TYPEID(void)
TYPEID(bool)
TYPEID(uint8_t)
TYPEID(uint16_t)
TYPEID(uint64_t)
TYPEID(uint256_t)

class Buffer;
TYPEID(Buffer)

class Equation;
TYPEID(Equation)

template <int SIZE>
class Bytes;
TYPEID_CUSTOM(Bytes<20>, Address)
TYPEID_CUSTOM(Bytes<32>, Hash)
TYPEID_CUSTOM(Bytes<64>, Signature)

#undef TYPEID_CUSTOM
#undef TYPEID
