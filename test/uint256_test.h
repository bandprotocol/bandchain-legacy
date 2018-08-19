#include <cxxtest/TestSuite.h>

#include "util/bytes.h"

class UnsignedInt256Test : public CxxTest::TestSuite
{
public:
  void testHello()
  {
    uint256_t n1 = 9'223'372'036'854'775'807;
    n1 *= 9'223'372'036'854'775'807;
    auto x = BigInt::from_uint256(n1);
    uint256_t n2 = x.as_uint256();
    TS_ASSERT_EQUALS(n1, n2);
    log::info("yo {}", n1);
    log::info("hello {}", x);

    auto y = Bytes<1>::from_hex("f1");
    for (size_t i = 0; i < 8; ++i) {
      log::info("{}: {}", i, y.get_bit(i));
    }
    // TS_ASSERT_EQUALS(number, 21);
  }
};
