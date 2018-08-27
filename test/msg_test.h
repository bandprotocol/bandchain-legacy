#include <cxxtest/TestSuite.h>

#include "msg/msg.h"

class MsgTest : public CxxTest::TestSuite
{
public:
  void test_msg_verify()
  {
    auto raw = Bytes<116>::from_hex(
        "0000000001643f1587c800000000000000016ddb22994b551f4da5818e7a257d467e9a"
        "f753348194f31dddc5f9aa489d3da10691549a32719e4ef82796309a1e6d76dc400daa"
        "da178eca715def000f655b777b67a1b374ce546400ded48cc2112c1a550168ac659e71"
        "5060b314ceb8ef7200ffff");
    auto vk = VerifyKey::from_hex(
        "6ddb22994b551f4da5818e7a257d467e9af753348194f31dddc5f9aa489d3da1");

    Buffer buf;
    buf << raw;

    MsgHdr hdr;
    buf >> hdr;

    TS_ASSERT_EQUALS(hdr.msgid, MsgID(0));
    TS_ASSERT_EQUALS(hdr.ts, 1530066733000);
    TS_ASSERT_EQUALS(hdr.nonce, 1);
    TS_ASSERT_EQUALS(hdr.vk, vk);

    TS_ASSERT_EQUALS(buf.size_bytes(), 2);
    TS_ASSERT(hdr.verify_signature(buf));

    hdr.sig[0] = std::byte{0x00};
    TS_ASSERT(!hdr.verify_signature(buf));
  }

  void test_msg_hdr()
  {
    MsgHdr hdr;
    hdr.msgid = MsgID::Unset;
    hdr.ts = 1530066733000;
    hdr.nonce = 1;
    hdr.vk = VerifyKey::rand();
    hdr.sig = Signature::rand();

    Buffer buf;
    TS_ASSERT_EQUALS(buf.size_bytes(), 0);

    buf << hdr;
    TS_ASSERT_EQUALS(buf.size_bytes(), 114);
    log::info("HEY {}", buf);

    MsgHdr hdr_read;
    buf >> hdr_read;

    TS_ASSERT_EQUALS(buf.size_bytes(), 0);
    TS_ASSERT_EQUALS(hdr.msgid, hdr_read.msgid);
    TS_ASSERT_EQUALS(hdr.ts, hdr_read.ts);
    TS_ASSERT_EQUALS(hdr.nonce, hdr_read.nonce);
    TS_ASSERT_EQUALS(hdr.vk, hdr_read.vk);
    TS_ASSERT_EQUALS(hdr.sig, hdr_read.sig);
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
