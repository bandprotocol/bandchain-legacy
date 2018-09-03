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

    // Consume first byte
    buf.consume(1);
    TS_ASSERT_EQUALS(2, buf.size_bytes());
    TS_ASSERT_EQUALS(*buf.begin(), b);

    buf.append(d);
    buf.append(a);

    // Consume first 2 bytes
    buf.consume(2);

    // Remain d and a in buffer
    TS_ASSERT_EQUALS(*buf.begin(), d);
    TS_ASSERT_EQUALS(buf[1], a);
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
    // Allocate memory 39 bytes back of buffer
    buf.reserve(39);
    TS_ASSERT_EQUALS(42, buf.size_bytes());
  }

  void testpush(void)
  {
    // // Test << and >> operation
    // std::byte d{0xDD};

    // Bytes<32> A{0xAAA};
    // Buffer buf;
    // uint8_t b = 0xBB;
    // // Push A (32 bytes data) to buffer
    // buf << A;
    // TS_ASSERT_EQUALS(32, buf.size_bytes());

    // // Push b (1 byte data) to buffer
    // buf << b;
    // TS_ASSERT_EQUALS(33, buf.size_bytes());
    // log::info("Buffer now {}",buf);

    // // Pop 32 first byte of buffer store in B
    // Bytes<32> B{0x00};
    // buf >> B;
    // TS_ASSERT_EQUALS(1, buf.size_bytes());
    // TS_ASSERT_EQUALS(A,B);
    // // Show buffer now
    // log::info("Buffer now {}",buf);

    // // Add another 32 bytes data
    // Bytes<32> A17{0x5324};
    // buf << A17;
    // TS_ASSERT_EQUALS(33, buf.size_bytes());

    // uint16_t n16{0xB123};
    // buf << n16;
    // TS_ASSERT_EQUALS(35, buf.size_bytes());

    // uint8_t  c;
    // buf >> c;
    // TS_ASSERT_EQUALS(34, buf.size_bytes());
    // TS_ASSERT_EQUALS(c, 187);

    // Bytes<32> X;
    // buf >> X;
    // TS_ASSERT_EQUALS(2, buf.size_bytes());
    // TS_ASSERT_EQUALS(X, Bytes<32>{0x5324});

    // uint16_t n16_new;
    // buf >> n16_new;
    // TS_ASSERT_EQUALS(0, buf.size_bytes());
    // TS_ASSERT_EQUALS(n16, n16_new);
    // // log::info("Begin {}", *buf.begin());
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
    log::info("Buffer now {}", buf);

    // Vector of 17 bytes data size equal 2
    std::vector<Bytes<17>> rec(2);

    // Pop follow input span
    // Buffer will pop 2 first 17 bytes data and store to rec
    buf >> gsl::make_span(rec);
    // Left last element in buffer
    TS_ASSERT_EQUALS(17, buf.size_bytes());

    log::info("Buffer now {}", buf);
    TS_ASSERT_EQUALS(a, rec[0]);
    TS_ASSERT_EQUALS(b, rec[1]);

    // Check throw error if pop more than elements in buffer
    TS_ASSERT_THROWS_ANYTHING(buf >> gsl::make_span(rec));

    // Size must remain 17 no pop execute
    TS_ASSERT_EQUALS(17, buf.size_bytes());
  }
};
