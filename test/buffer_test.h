#include <cxxtest/TestSuite.h>
#include "util/buffer.h"
#include <vector>

class BufferTest : public CxxTest::TestSuite
{
public:
  void testCreateBuffer(void)
  {
    Buffer buf;
    TS_ASSERT_EQUALS(0, buf.size());
  }

  void testAppendOneElementBuffer(void)
  {
    Buffer buf;
    std::byte A{0x55};
    buf.append(A);
    // Check size
    TS_ASSERT_EQUALS(1, buf.size());

    std::byte B{0xAB};
    buf.append(B);
    // Check size and []
    TS_ASSERT_EQUALS(2, buf.size());
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

    TS_ASSERT_EQUALS(3, buf.size());
    buf.consume(1);
    TS_ASSERT_EQUALS(2, buf.size());
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

    a = static_cast<std::byte> (0x72);
    V.push_back(a);
    a = static_cast<std::byte> (0xDD);
    V.push_back(a);
    a = static_cast<std::byte> (0xCD);
    V.push_back(a);
    
    Buffer buf;
    buf.append(&V[1],2);
    TS_ASSERT_EQUALS(2, buf.size());
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

    TS_ASSERT_EQUALS(4, buf2.size());
    TS_ASSERT_EQUALS(*buf2.begin(), std::byte{0xAA});
    TS_ASSERT_EQUALS(buf2[3], std::byte{0xDD});

    buf2.consume(2);
    TS_ASSERT_EQUALS(2, buf2.size());
    TS_ASSERT_EQUALS(*buf2.begin(), std::byte{0xCC});
    TS_ASSERT_EQUALS(buf2[1], std::byte{0xDD});

    // test clear
    buf.clear();
    TS_ASSERT_EQUALS(2, buf2.size());
    TS_ASSERT_EQUALS(0, buf.size());
    //How to check?
    //TS_ASSERT_EQUALS(buf.begin(), nullptr);
  }

  void testReserve()
  {
    std::byte a{0xAA};
    std::byte b{0xBB};
    std::byte c{0xCC};
    std::byte d{0xDD};

    Buffer buf;
    buf.append(a);
    buf.append(b);
    buf.append(c);

    TS_ASSERT_EQUALS(3, buf.size());
    buf.reserve(39);
    TS_ASSERT_EQUALS(42, buf.size());
  }
};
