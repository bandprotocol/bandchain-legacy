#pragma once

#include "util/buffer.h"
#include "util/bytes.h"
#include "util/endian.h"

enum class ObjectID : uint16_t {
  Unset = 0,
  Tx = 1,
  Mint = 2,
  Account = 32768,
  Test = 65535,
};

class Object
{
public:
  /// Load an object from the given buffer. Throw on failure.
  static std::unique_ptr<Object> load(Buffer& buf);

  /// Dump this object to a buffer.
  Buffer& dump(Buffer& buf) const { return serialize_impl(buf); }

  /// Return the hash of this object. Must probabilistically be unique.
  virtual Hash hash() const = 0;

  /// Return the key of this object. Throw if the object cannot be stored in
  /// state tree.
  virtual Hash key() const
  {
    throw Error("Object at {} does not have key attribute", hash());
  }

  /// Cast this object into the provided type. Throw on failure.
  template <typename T,
            typename = std::enable_if_t<std::is_base_of_v<Object, T>>>
  const T& as()
  {
    const T* object = dynamic_cast<const T*>(this);
    if (object == nullptr) {
      throw Error("Object at {} has unexpected type", hash());
    }
    return *object;
  }

private:
  /// Default implementation throws. Specialize it yourselves!
  template <ObjectID OID>
  static std::unique_ptr<Object> deserialize(Buffer& buf)
  {
    throw Error("Cannot load object of type {}", static_cast<uint16_t>(OID));
  }

  /// Implemented in ObjectBase.
  virtual Buffer& serialize_impl(Buffer& buf) const = 0;
};

template <typename ObjectType, ObjectID OID>
class ObjectBase : public Object
{
public:
  static constexpr ObjectID ID = OID;

  static std::unique_ptr<Object> deserialize_impl(Buffer& buf)
  {
    auto object = std::make_unique<ObjectType>();
    buf >>= *object;
    return object;
  }

private:
  /// Serialize the ID then the object itself.
  Buffer& serialize_impl(Buffer& buf) const final
  {
    return buf << uint16_t(OID) <<= static_cast<const ObjectType&>(*this);
  }
};
