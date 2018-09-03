#include <cxxtest/TestSuite.h>

#include "band/msg.h"
#include "inc/essential.h"

class MsgTest : public CxxTest::TestSuite
{
public:
  void test_msg_hdr()
  {
    MsgHdr hdr;
    hdr.msgid = MsgID::Unset;
    hdr.ts = 1530066733000;
    hdr.vk = VerifyKey::rand();

    Buffer buf;
    TS_ASSERT_EQUALS(buf.size_bytes(), 0);

    buf << hdr;
    TS_ASSERT_EQUALS(buf.size_bytes(), 42);
    log::info("HEY {}", buf);

    MsgHdr hdr_read;
    buf >> hdr_read;

    TS_ASSERT_EQUALS(buf.size_bytes(), 0);
    TS_ASSERT_EQUALS(hdr.msgid, hdr_read.msgid);
    TS_ASSERT_EQUALS(hdr.ts, hdr_read.ts);
    TS_ASSERT_EQUALS(hdr.vk, hdr_read.vk);
  }

  void test_msg_mint()
  {
    TS_ASSERT_EQUALS(MintMsg::ID, MsgID::Mint);

    MintMsg msg;
    msg.value = 1000'000'000'000'000;

    Buffer buf;
    TS_ASSERT_EQUALS(buf.size_bytes(), 0);

    buf << msg;
    TS_ASSERT_EQUALS(buf.size_bytes(), 32);

    MintMsg msg_read;
    buf >> msg_read;

    TS_ASSERT_EQUALS(buf.size_bytes(), 0);
    TS_ASSERT_EQUALS(msg.value, msg_read.value);
  }

  void test_msg_tx()
  {
    TS_ASSERT_EQUALS(TxMsg::ID, MsgID::Tx);

    TxMsg msg;
    msg.dest = Address::rand();
    msg.value = 1000'000'000'000'000;

    Buffer buf;
    TS_ASSERT_EQUALS(buf.size_bytes(), 0);

    buf << msg;
    TS_ASSERT_EQUALS(buf.size_bytes(), 52);

    TxMsg msg_read;
    buf >> msg_read;

    TS_ASSERT_EQUALS(buf.size_bytes(), 0);
    TS_ASSERT_EQUALS(msg.dest, msg_read.dest);
    TS_ASSERT_EQUALS(msg.value, msg_read.value);
  }
};
