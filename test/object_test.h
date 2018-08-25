#include <cxxtest/TestSuite.h>

#include "state/object.h"
#include "util/string.h"

class TestObject : public ObjectBase<TestObject, ObjectID::Test>
{
public:
private:
  Hash hash() const final { return Hash(); }
};

inline Buffer& operator<<(Buffer& buf, const TestObject& val) { return buf; }
inline Buffer& operator>>(Buffer& buf, TestObject& val) { return buf; }

template <>
std::unique_ptr<Object> Object::deserialize<TestObject::ID>(Buffer& buf)
{
  auto object = std::make_unique<TestObject>();
  buf >> *object;
  return object;
}

class ObjectTest : public CxxTest::TestSuite
{
public:
  void testSimple()
  {
    Buffer buf;

    uint256_t x = 1024;
    buf << x;
    log::info("1: {}", buf);
    log::info("1: {}", x);

    uint16_t y = 12;
    buf << y;
    log::info("2: {}", buf);
    log::info("2: {}", y);

    uint256_t a;
    buf >> a;
    log::info("3: {}", buf);
    log::info("3: {}", a);

    uint16_t b;
    buf >> b;
    log::info("4: {}", buf);
    log::info("4: {}", b);
  }
};
