#include "object.h"

std::unique_ptr<Object> Object::load(Buffer& buf)
{
  uint16_t object_id = 0;
  buf >> object_id;

  switch (ObjectID(object_id)) {
    case ObjectID::Unset:
      return Object::deserialize<ObjectID::Unset>(buf);
    case ObjectID::Test:
      return Object::deserialize<ObjectID::Test>(buf);

      // No default case.
  }
  throw Error("Unable to load object with unexpected ID {}", object_id);
}