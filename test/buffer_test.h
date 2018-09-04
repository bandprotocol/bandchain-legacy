#include <cxxtest/TestSuite.h>
#include <vector>

#include "inc/essential.h"
#include "state/object.h"
#include "util/buffer.h"
#include "util/bytes.h"

class BufferTest : public CxxTest::TestSuite
{
public:
  void testCreateBuffer(void)
  {
    Buffer buf;
    TS_ASSERT_EQUALS(0, buf.size_bytes());
  }

  void testAppendOneElementBuffer(void)
  {
    Buffer buf;
    std::byte A{0x55};
    buf << A;
    // Check size_bytes
    TS_ASSERT_EQUALS(1, buf.size_bytes());

    std::byte B{0xAB};
    buf << B;
    // Check size_bytes and []
    TS_ASSERT_EQUALS(2, buf.size_bytes());
    TS_ASSERT_EQUALS(buf.as_span()[0], A);
    TS_ASSERT_EQUALS(buf.as_span()[1], B);
  }

  void testConsume(void)
  {
    std::byte a{0xAA};
    std::byte b{0xBB};
    std::byte c{0xCC};
    std::byte d{0xDD};

    Buffer buf;
    buf << a << b << c;

    TS_ASSERT_EQUALS(3, buf.size_bytes());

    // Consume first byte
    buf.consume(1);
    TS_ASSERT_EQUALS(2, buf.size_bytes());
    TS_ASSERT_EQUALS(buf.as_span()[0], b);

    buf << d << a;

    // Consume first 2 bytes
    buf.consume(2);

    // Remain d and a in buffer
    TS_ASSERT_EQUALS(buf.as_span()[0], d);
    TS_ASSERT_EQUALS(buf.as_span()[1], a);
  }

  void testAppendSpanType(void)
  {
    std::vector<std::byte> V;
    std::byte a;

    a = static_cast<std::byte>(0xDD);
    V.push_back(a);
    a = static_cast<std::byte>(0xCD);
    V.push_back(a);

    Buffer buf;
    // Push span from vector to buffer
    buf << gsl::make_span(V);
    TS_ASSERT_EQUALS(2, buf.size_bytes());
    TS_ASSERT_EQUALS(buf.as_span()[0], std::byte{0xDD});
    TS_ASSERT_EQUALS(buf.as_span()[1], std::byte{0xCD});
  }

  void testAppendOtherBuffer(void)
  {
    std::byte a{0xAA};
    std::byte b{0xBB};
    std::byte c{0xCC};
    std::byte d{0xDD};

    Buffer buf;
    buf << a << b << c;

    Buffer buf2;
    buf2 << buf.as_span() << d;

    TS_ASSERT_EQUALS(4, buf2.size_bytes());
    TS_ASSERT_EQUALS(buf2.as_span()[0], std::byte{0xAA});
    TS_ASSERT_EQUALS(buf2.as_span()[3], std::byte{0xDD});

    buf2.consume(2);
    TS_ASSERT_EQUALS(2, buf2.size_bytes());
    TS_ASSERT_EQUALS(buf2.as_span()[0], std::byte{0xCC});
    TS_ASSERT_EQUALS(buf2.as_span()[1], std::byte{0xDD});

    // test clear
    buf.clear();
    TS_ASSERT_EQUALS(2, buf2.size_bytes());
    TS_ASSERT_EQUALS(0, buf.size_bytes());
  }

  void testAddOtherSpanToBuffer(void)
  {
    std::vector<Bytes<17>> address;
    Bytes<17> a = Bytes<17>::from_hex("942e2c5433b264848aabe4b5e8cb154ba5");
    address.push_back(a);
    Bytes<17> b = Bytes<17>::from_hex("cc902326010122eed5afe07fbb1fed7f71");
    address.push_back(b);
    Bytes<17> c = Bytes<17>::from_hex("7735a79992f135fdbcc81e10fe814d1362");
    address.push_back(c);

    Buffer buf;
    // Push span of this vector to buffer
    buf << gsl::make_span(address);
    TS_ASSERT_EQUALS(51, buf.size_bytes());

    // Vector of 17 bytes data size equal 2
    std::vector<Bytes<17>> rec(2);

    // Pop follow input span
    // Buffer will pop 2 first 17 bytes data and store to rec
    buf >> gsl::make_span(rec);
    // Left last element in buffer
    TS_ASSERT_EQUALS(17, buf.size_bytes());

    TS_ASSERT_EQUALS(a, rec[0]);
    TS_ASSERT_EQUALS(b, rec[1]);

    // Check throw error if pop more than elements in buffer
    TS_ASSERT_THROWS_ANYTHING(buf >> gsl::make_span(rec));

    // Size must remain 17 no pop execute
    TS_ASSERT_EQUALS(17, buf.size_bytes());
  }
};
