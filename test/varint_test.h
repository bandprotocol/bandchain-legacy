#include <cxxtest/TestSuite.h>

#include "inc/essential.h"
#include "util/buffer.h"

class VarintTest : public CxxTest::TestSuite
{
public:
  void testVarintEncode()
  {
    uint32_t val = 2000;
    Buffer buf;
    buf << val;
    TS_ASSERT_EQUALS(buf.to_string(), "d00f");
  }

  void testVarintDecode()
  {
    Buffer buf;
    buf << std::byte{0xac} << std::byte{0x02};
    uint32_t val;
    buf >> val;
    TS_ASSERT_EQUALS(val, 300);
  }

  void testVarintEncodeDecode()
  {
    uint256_t val = 1000000000000;
    Buffer buf;
    buf << val << std::byte{0xAB};
    uint256_t decoded_val;
    buf >> decoded_val;

    TS_ASSERT_EQUALS(val, decoded_val);
    TS_ASSERT_EQUALS(buf.size_bytes(), 1);
  }
};
