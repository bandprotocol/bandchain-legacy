#include <cxxtest/TestSuite.h>
#include <vector>

#include "state/object.h"
#include "util/bytes.h"
#include "util/buffer.h"

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
    buf.append(A);
    // Check size_bytes
    TS_ASSERT_EQUALS(1, buf.size_bytes());

    std::byte B{0xAB};
    buf.append(B);
    // Check size_bytes and []
    TS_ASSERT_EQUALS(2, buf.size_bytes());
    TS_ASSERT_EQUALS(buf[1], B);

    // Check begin
    auto it = buf.begin();
    TS_ASSERT_EQUALS(A, *it);
  }

  void testConsume(void)
  {
    std::byte a{0xAA};
    std::byte b{0xBB};
    std::byte c{0xCC};
    std::byte d{0xDD};

    Buffer buf;
    buf.append(a);
    buf.append(b);
    buf.append(c);

    TS_ASSERT_EQUALS(3, buf.size_bytes());
    buf.consume(1);
    TS_ASSERT_EQUALS(2, buf.size_bytes());
    TS_ASSERT_EQUALS(*buf.begin(), b);

    buf.append(d);
    buf.append(a);
    buf.consume(2);
    TS_ASSERT_EQUALS(*buf.begin(), d);
    TS_ASSERT_EQUALS(buf[1], a);
  }

  void testAppendSpanType(void)
  {
    std::vector<std::byte> V;
    // V.emplace_back(0x72);
    // V.emplace_back(0xDD);
    // V.emplace_back(0xCD);
    std::byte a;

    a = static_cast<std::byte>(0x72);
    //V.push_back(a);
    a = static_cast<std::byte>(0xDD);
    V.push_back(a);
    a = static_cast<std::byte>(0xCD);
    V.push_back(a);

    Buffer buf;
    buf << gsl::make_span(V);
    TS_ASSERT_EQUALS(2, buf.size_bytes());
    TS_ASSERT_EQUALS(*buf.begin(), std::byte{0xDD});
    TS_ASSERT_EQUALS(buf[1], std::byte{0xCD});
  }

  void testAppendOtherBuffer(void)
  {
    std::byte a{0xAA};
    std::byte b{0xBB};
    std::byte c{0xCC};
    std::byte d{0xDD};

    Buffer buf;
    buf.append(a);
    buf.append(b);
    buf.append(c);

    Buffer buf2;
    buf2.append(buf);
    buf2.append(d);

    TS_ASSERT_EQUALS(4, buf2.size_bytes());
    TS_ASSERT_EQUALS(*buf2.begin(), std::byte{0xAA});
    TS_ASSERT_EQUALS(buf2[3], std::byte{0xDD});

    buf2.consume(2);
    TS_ASSERT_EQUALS(2, buf2.size_bytes());
    TS_ASSERT_EQUALS(*buf2.begin(), std::byte{0xCC});
    TS_ASSERT_EQUALS(buf2[1], std::byte{0xDD});

    // test clear
    buf.clear();
    TS_ASSERT_EQUALS(2, buf2.size_bytes());
    TS_ASSERT_EQUALS(0, buf.size_bytes());
    // How to check?
    // TS_ASSERT_EQUALS(buf.begin(), nullptr);
  }

  void testReserve(void)
  {
    std::byte a{0xAA};
    std::byte b{0xBB};
    std::byte c{0xCC};
    std::byte d{0xDD};

    Buffer buf;
    buf.append(a);
    buf.append(b);
    buf.append(c);

    TS_ASSERT_EQUALS(3, buf.size_bytes());
    buf.reserve(39);
    TS_ASSERT_EQUALS(42, buf.size_bytes());
  }

  void testpush(void)
  {
    //std::byte a{0xAA};
    //std::byte b{0xBB};
    // std::byte c{0xCC};
    std::byte d{0xDD};

    Bytes<32> A{0xAAA};
    Buffer buf;
    uint8_t b = 0xBB; 
    buf << A;
    TS_ASSERT_EQUALS(32, buf.size_bytes());

    buf << b;
    TS_ASSERT_EQUALS(33, buf.size_bytes());
    log::info("Buffer now {}",buf);
    //buf.append(a);
    Bytes<32> B{0x00};
    buf >> B;
    TS_ASSERT_EQUALS(1, buf.size_bytes());
    TS_ASSERT_EQUALS(A,B);
    log::info("Buffer now {}",buf);
    Bytes<32> A17{0x5324};
    buf << A17;

    TS_ASSERT_EQUALS(33, buf.size_bytes());
    uint8_t  c;
    buf >> c;
    TS_ASSERT_EQUALS(32, buf.size_bytes());
    TS_ASSERT_EQUALS(c, 187);

    Bytes<32> X;
    buf >> X;
    TS_ASSERT_EQUALS(0, buf.size_bytes());
    TS_ASSERT_EQUALS(X, Bytes<32>{0x5324});
    // log::info("Begin {}", *buf.begin());
  }

};
