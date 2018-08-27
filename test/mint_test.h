#include <cxxtest/TestSuite.h>

#include "msg/mint.h"

class MintTest : public CxxTest::TestSuite
{
public:
  void testPushToBuffer(void)
  {
    Buffer write_buf;
    MintMsg mint;
    mint.value = 1000;
    mint.addr = Address::from_hex("2de9bf7fc075cc48a3f799f5138ca1708f4d0e2a");
    mint.ident = Hash::from_hex(
        "4cc65c5b6d75d81d0a58ca56b3cb73bc95c59fb19bcc06c6950143d9bd050583");

    write_buf <<= mint;
    TS_ASSERT_EQUALS(84, write_buf.size_bytes());

    Address a1;
    write_buf >> a1;
    TS_ASSERT_EQUALS(a1, mint.addr);

    uint256_t va1;
    write_buf >> va1;
    TS_ASSERT_EQUALS(va1, mint.value);

    Hash h1;
    write_buf >> h1;
    TS_ASSERT_EQUALS(h1, mint.ident);

    TS_ASSERT_EQUALS(0, write_buf.size_bytes());

    write_buf <<= mint;

    TS_ASSERT_EQUALS(84, write_buf.size_bytes());
    // Change Order

    write_buf >> va1;
    TS_ASSERT(va1 != mint.value);

    write_buf >> a1;
    TS_ASSERT(a1 != mint.addr);

    write_buf >> h1;
    TS_ASSERT_EQUALS(h1, mint.ident);

    TS_ASSERT_EQUALS(0, write_buf.size_bytes());
    // Put Data to buf
  }

  void testDump(void)
  {
    Buffer write_buf;
    MintMsg mint;
    log::info("Size = {}", sizeof(MintMsg));
    mint.value = 1000;
    mint.addr = Address::from_hex("2de9bf7fc075cc48a3f799f5138ca1708f4d0e2a");
    mint.ident = Hash::from_hex(
        "4cc65c5b6d75d81d0a58ca56b3cb73bc95c59fb19bcc06c6950143d9bd050583");

    mint.dump(write_buf);
    TS_ASSERT_EQUALS(86, write_buf.size_bytes());

    // Check Type
    uint16_t type;
    write_buf >> type;
    TS_ASSERT_EQUALS(type, 2);
    TS_ASSERT_EQUALS(84, write_buf.size_bytes());

    Address a1;
    write_buf >> a1;
    TS_ASSERT_EQUALS(a1, mint.addr);

    uint256_t va1;
    write_buf >> va1;
    TS_ASSERT_EQUALS(va1, mint.value);

    Hash h1;
    write_buf >> h1;
    TS_ASSERT_EQUALS(h1, mint.ident);

    TS_ASSERT_EQUALS(0, write_buf.size_bytes());
  }

  void testReadFromBuffer(void)
  {
    Buffer read_buf;
    uint256_t value = 1000;
    Address addr =
        Address::from_hex("2de9bf7fc075cc48a3f799f5138ca1708f4d0e2a");
    Hash ident = Hash::from_hex(
        "4cc65c5b6d75d81d0a58ca56b3cb73bc95c59fb19bcc06c6950143d9bd050583");

    // Create Buffer
    read_buf << addr;
    read_buf << value;
    read_buf << ident;

    MintMsg mint;
    read_buf >>= mint;
    TS_ASSERT_EQUALS(value, mint.value);
    TS_ASSERT_EQUALS(addr, mint.addr);
    TS_ASSERT_EQUALS(ident, mint.ident);
    TS_ASSERT_EQUALS(0, read_buf.size_bytes());
  }

  void testLoadAndAs(void)
  {
    Buffer read_buf;
    uint256_t value = 1000;
    Address addr =
        Address::from_hex("2de9bf7fc075cc48a3f799f5138ca1708f4d0e2a");
    Hash ident = Hash::from_hex(
        "4cc65c5b6d75d81d0a58ca56b3cb73bc95c59fb19bcc06c6950143d9bd050583");

    // Create Buffer
    // Add Type of TxMsg
    uint16_t type = 2;
    read_buf << type;

    read_buf << addr;
    read_buf << value;
    read_buf << ident;

    std::unique_ptr<Object> p1 = Object::load(read_buf);
    const MintMsg& mint = p1->as<MintMsg>();
    TS_ASSERT_EQUALS(value, mint.value);
    TS_ASSERT_EQUALS(addr, mint.addr);
    TS_ASSERT_EQUALS(ident, mint.ident);
    TS_ASSERT_EQUALS(0, read_buf.size_bytes());
  }

  //   void testVerifySignature(void)
  //   {
  //     // Create signature from data
  //     //VerifyKey vk =
  //     VerifyKey::from_hex("6ddb22994b551f4da5818e7a257d467e9af753348194f31dddc5f9aa489d3da1");
  //     uint256_t value =
  //     uint256_t::from_hex("da73e9ed53fc63c3d8fca9b027119681dc26439e85613bfb0b4870304642c487");
  //     uint256_t nonce =
  //     uint256_t::from_hex("0000000000000000000000000000000000000000000000000000000000000001");
  //     Address addr =
  //     Address::from_hex("2de9bf7fc075cc48a3f799f5138ca1708f4d0e2a");

  //     auto VSKey = ed25519_generate_keypair();
  //     VerifyKey vk = VSKey.first;
  //     Bytes<116> all = vk + value + nonce + addr;
  //     Signature sig = ed25519_sign(VSKey.second, gsl::span(all.data(), 116));
  //     log::info("Signature {}",sig);

  //     TxMsg tx;
  //     tx.vk = vk;
  //     tx.value = value;
  //     tx.nonce = nonce;
  //     tx.addr = addr;
  //     tx.sig = sig;

  //     TS_ASSERT(tx.verify());

  //     // Try to change address
  //     addr = Address::from_hex("6b1a55b55495b731cbe9eff75f73452f70d329b9");
  //     tx.addr = addr;
  //     TS_ASSERT(!tx.verify());

  //   }
};
