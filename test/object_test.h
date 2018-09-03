#include <cxxtest/TestSuite.h>

#include "inc/essential.h"
#include "state/object.h"
#include "util/bytes.h"
#include "util/string.h"

class TestObject : public ObjectBase<TestObject, ObjectID::Test>
{
public:
  friend Buffer& operator<<=(Buffer& buf, const TestObject& obj)
  {
    return buf << obj.my_data;
  }

  friend Buffer& operator>>=(Buffer& buf, TestObject& obj)
  {
    return buf >> obj.my_data;
  }

  Hash hash() const final { return Hash(); }

  uint32_t my_data = 0;
};

template <>
std::unique_ptr<Object> Object::deserialize<TestObject::ID>(Buffer& buf)
{
  return TestObject::deserialize_impl(buf);
}

class ObjectTest : public CxxTest::TestSuite
{
public:
  void testObjectSimple()
  {
    Buffer buf;
    TestObject t1;
    t1.my_data = 12;

    t1.dump(buf);
    log::info("0: {}", buf);
    log::info("0: {}", t1.my_data);

    std::unique_ptr<Object> t2 = Object::load(buf);
    log::info("1: {}", buf);
    const auto& t3 = t2->as<TestObject>();
    log::info("1: {}", t3.my_data);
  }

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

  void testBytes()
  {
    Buffer buf;

    auto b1 = Bytes<2>::from_hex("45fa");
    buf << b1;
    log::info("1: {}", buf);
    log::info("1: {}", b1);

    Bytes<2> b2;
    buf >> b2;
    log::info("2: {}", buf);
    log::info("2: {}", b2);
  }
};
