#include <cxxtest/TestSuite.h>

#include "msg/tx.h"

class TxTest : public CxxTest::TestSuite
{
public:
  void testPushToBuffer(void)
  {
    Buffer write_buf;
    TxMsg tx;
    tx.vk = VerifyKey::from_hex("6ddb22994b551f4da5818e7a257d467e9af753348194f31dddc5f9aa489d3da1");
    tx.value = BigInt::from_hex("da73e9ed53fc63c3d8fca9b027119681dc26439e85613bfb0b4870304642c487");
    tx.nonce = BigInt::from_hex("0000000000000000000000000000000000000000000000000000000000000001");
    tx.addr = Address::from_hex("2de9bf7fc075cc48a3f799f5138ca1708f4d0e2a");
    // Fake (Random) Signature
    tx.sig = Signature::from_hex("210f5e820248bb1f159070043e2888f91bafe11a74c5bda2e60ab76407bc9b0969a95b3784410f511d37cc02316e77c46f1196fe2eb5f5dcaf2fc7ae45d45bf8");

    write_buf <<= tx;
    TS_ASSERT_EQUALS(180,write_buf.size_bytes());

    VerifyKey v1;
    write_buf >> v1;
    TS_ASSERT_EQUALS(v1, tx.vk);

    BigInt va1;
    write_buf >> va1;
    TS_ASSERT_EQUALS(va1, tx.value);

    BigInt n1;
    write_buf >> n1;
    TS_ASSERT_EQUALS(n1, tx.nonce);

    Address a1;
    write_buf >> a1;
    TS_ASSERT_EQUALS(a1, tx.addr);

    Signature s1;
    write_buf >> s1;
    TS_ASSERT_EQUALS(s1, tx.sig);

    TS_ASSERT_EQUALS(0, write_buf.size_bytes());

    write_buf <<= tx;

    TS_ASSERT_EQUALS(180,write_buf.size_bytes());
    // Change Order
    write_buf >> v1;
    TS_ASSERT_EQUALS(v1, tx.vk);

    write_buf >> a1;
    TS_ASSERT(a1 != tx.addr);

    write_buf >> va1;
    TS_ASSERT(va1 != tx.value);

    write_buf >> n1;
    TS_ASSERT(n1 != tx.nonce);

    write_buf >> s1;
    TS_ASSERT_EQUALS(s1, tx.sig);

    TS_ASSERT_EQUALS(0, write_buf.size_bytes());
    // Put Data to buf
  }

  void testDump(void)
  {
    Buffer write_buf;
    TxMsg tx;
    tx.vk = VerifyKey::from_hex("6ddb22994b551f4da5818e7a257d467e9af753348194f31dddc5f9aa489d3da1");
    tx.value = BigInt::from_hex("da73e9ed53fc63c3d8fca9b027119681dc26439e85613bfb0b4870304642c487");
    tx.nonce = BigInt::from_hex("0000000000000000000000000000000000000000000000000000000000000001");
    tx.addr = Address::from_hex("2de9bf7fc075cc48a3f799f5138ca1708f4d0e2a");
    // Fake (Random) Signature
    tx.sig = Signature::from_hex("210f5e820248bb1f159070043e2888f91bafe11a74c5bda2e60ab76407bc9b0969a95b3784410f511d37cc02316e77c46f1196fe2eb5f5dcaf2fc7ae45d45bf8");

    tx.dump(write_buf);
    TS_ASSERT_EQUALS(182,write_buf.size_bytes());
    
    // Check Type
    uint16_t type;
    write_buf >> type;
    TS_ASSERT_EQUALS(type, 1);
    TS_ASSERT_EQUALS(180,write_buf.size_bytes());

    VerifyKey v1;
    write_buf >> v1;
    TS_ASSERT_EQUALS(v1, tx.vk);

    BigInt va1;
    write_buf >> va1;
    TS_ASSERT_EQUALS(va1, tx.value);

    BigInt n1;
    write_buf >> n1;
    TS_ASSERT_EQUALS(n1, tx.nonce);

    Address a1;
    write_buf >> a1;
    TS_ASSERT_EQUALS(a1, tx.addr);

    Signature s1;
    write_buf >> s1;
    TS_ASSERT_EQUALS(s1, tx.sig);

    TS_ASSERT_EQUALS(0, write_buf.size_bytes());
  }

  void testReadFromBuffer(void)
  {
    Buffer read_buf;
    VerifyKey vk = VerifyKey::from_hex("6ddb22994b551f4da5818e7a257d467e9af753348194f31dddc5f9aa489d3da1");
    BigInt value = BigInt::from_hex("da73e9ed53fc63c3d8fca9b027119681dc26439e85613bfb0b4870304642c487");
    BigInt nonce = BigInt::from_hex("0000000000000000000000000000000000000000000000000000000000000001");
    Address addr = Address::from_hex("2de9bf7fc075cc48a3f799f5138ca1708f4d0e2a");
    // Fake (Random) Signature
    Signature sig = Signature::from_hex("210f5e820248bb1f159070043e2888f91bafe11a74c5bda2e60ab76407bc9b0969a95b3784410f511d37cc02316e77c46f1196fe2eb5f5dcaf2fc7ae45d45bf8");

    //Create Buffer
    read_buf << vk;
    read_buf << value;
    read_buf << nonce;
    read_buf << addr;
    read_buf << sig;

    TxMsg tx;
    read_buf >>= tx;
    TS_ASSERT_EQUALS(vk, tx.vk);
    TS_ASSERT_EQUALS(value, tx.value);
    TS_ASSERT_EQUALS(nonce, tx.nonce);
    TS_ASSERT_EQUALS(addr, tx.addr);
    TS_ASSERT_EQUALS(sig, tx.sig);
    TS_ASSERT_EQUALS(0, read_buf.size_bytes());
  }

  void testLoadAndAs(void)
  {
    Buffer read_buf;
    VerifyKey vk = VerifyKey::from_hex("6ddb22994b551f4da5818e7a257d467e9af753348194f31dddc5f9aa489d3da1");
    BigInt value = BigInt::from_hex("da73e9ed53fc63c3d8fca9b027119681dc26439e85613bfb0b4870304642c487");
    BigInt nonce = BigInt::from_hex("0000000000000000000000000000000000000000000000000000000000000001");
    Address addr = Address::from_hex("2de9bf7fc075cc48a3f799f5138ca1708f4d0e2a");
    // Fake (Random) Signature
    Signature sig = Signature::from_hex("210f5e820248bb1f159070043e2888f91bafe11a74c5bda2e60ab76407bc9b0969a95b3784410f511d37cc02316e77c46f1196fe2eb5f5dcaf2fc7ae45d45bf8");

    //Create Buffer
    // Add Type of TxMsg
    uint16_t type = 1;
    read_buf << type;

    read_buf << vk;
    read_buf << value;
    read_buf << nonce;
    read_buf << addr;
    read_buf << sig;

    std::unique_ptr<Object> p1 = Object::load(read_buf);
    const TxMsg& tx = p1->as<TxMsg>();
    TS_ASSERT_EQUALS(vk, tx.vk);
    TS_ASSERT_EQUALS(value, tx.value);
    TS_ASSERT_EQUALS(nonce, tx.nonce);
    TS_ASSERT_EQUALS(addr, tx.addr);
    TS_ASSERT_EQUALS(sig, tx.sig);
    TS_ASSERT_EQUALS(0, read_buf.size_bytes());
  }

  void testVerifySignature(void)
  {
    // Create signature from data
    //VerifyKey vk = VerifyKey::from_hex("6ddb22994b551f4da5818e7a257d467e9af753348194f31dddc5f9aa489d3da1");
    BigInt value = BigInt::from_hex("da73e9ed53fc63c3d8fca9b027119681dc26439e85613bfb0b4870304642c487");
    BigInt nonce = BigInt::from_hex("0000000000000000000000000000000000000000000000000000000000000001");
    Address addr = Address::from_hex("2de9bf7fc075cc48a3f799f5138ca1708f4d0e2a");
  
    auto VSKey = ed25519_generate_keypair();
    VerifyKey vk = VSKey.first;
    Bytes<116> all = vk + value + nonce + addr;
    Signature sig = ed25519_sign(VSKey.second, gsl::span(all.data(), 116));
    log::info("Signature {}",sig);

    TxMsg tx;
    tx.vk = vk;
    tx.value = value;
    tx.nonce = nonce;
    tx.addr = addr;
    tx.sig = sig;

    TS_ASSERT(tx.verify());
  }
};
