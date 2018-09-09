#include <cxxtest/TestSuite.h>

#include "band/handler.h"
#include "band/msg.h"
#include "band/txgen.h"
#include "crypto/ed25519.h"
#include "store/account.h"
#include "store/context_map.h"
#include "util/buffer.h"
#include "util/equation.h"
#include "util/json.h"

class HandlerTest : public CxxTest::TestSuite
{
public:
  void testMint()
  {
    ContextMap ctx;
    Handler hand(ctx);
    MintMsg mint;
    mint.token_key =
        TokenKey::from_hex("0000000000000000000000000000000000000000");
    mint.value = 10000;

    Address addr = Address::rand();
    hand.apply_mint(addr, mint, Hash::rand());

    // Check account balance
    Account account(ctx, addr);
    TS_ASSERT_EQUALS(10000, account.get_band_balance());

    TS_ASSERT_EQUALS(10000, account.get_balance(mint.token_key));
  }

  void testPurchaseCT()
  {
    ContextMap ctx;
    Handler hand(ctx);

    Address addr = Address::rand();

    // Mint band token to tester
    MintMsg mint;
    mint.token_key =
        TokenKey::from_hex("0000000000000000000000000000000000000000");
    mint.value = 10000;
    hand.apply_mint(addr, mint, Hash::rand());

    // Create new community contract
    CreateMsg create;
    std::string m =
        R"foo({"expressions": ["ADD", "ADD", "MUL", "1", "EXP", "X", "2", "MUL", "2", "X", "0"],
                "max_supply": "20"})foo";
    json j = json::parse(m);

    // Create body msg of create_msg from  param "expression"
    std::string pa = txgen::process_create(j);

    // pa store real 00010002.... cannot read
    Buffer buf;
    buf << gsl::make_span(pa);

    buf >> create;
    Hash tx_hash = Hash::rand();
    ContractID contract_id = tx_hash.prefix<ContractID::Size>();
    hand.apply_create(addr, create, tx_hash);

    // Purchase token
    // Create view community contract
    CommunityContract contract(ctx, contract_id);
    PurchaseCTMsg pct;
    pct.amount = 10;
    pct.contract_id = contract_id;
    pct.band_limit = 150; // enough to buy

    hand.apply_purchaseCT(addr, pct, Hash::rand());

    // Test after purchase ct
    Account account(ctx, addr);
    TS_ASSERT_EQUALS(9880, account.get_band_balance());
    TS_ASSERT_EQUALS(10, account.get_balance(contract_id));

    TS_ASSERT_EQUALS(10, contract.get_current_supply());

    // Another tester
    Address addr2 = Address::rand();

    // Mint band token to tester2
    mint.token_key =
        TokenKey::from_hex("0000000000000000000000000000000000000000");
    mint.value = 100;
    hand.apply_mint(addr2, mint, Hash::rand());

    pct.amount = 2;
    pct.contract_id = contract_id;
    pct.band_limit = 10; // not enough to buy

    TS_ASSERT_THROWS_ANYTHING(hand.apply_purchaseCT(addr2, pct, Hash::rand()));

    Account account2(ctx, addr2);
    TS_ASSERT_EQUALS(100, account2.get_band_balance());
    TS_ASSERT_EQUALS(0, account2.get_balance(contract_id));

    TS_ASSERT_EQUALS(10, contract.get_current_supply());

    pct.band_limit = 100; // enought to buy;
    hand.apply_purchaseCT(addr2, pct, Hash::rand());

    TS_ASSERT_EQUALS(52, account2.get_band_balance());
    TS_ASSERT_EQUALS(2, account2.get_balance(contract_id));
    TS_ASSERT_EQUALS(12, contract.get_current_supply());

    // Buy 10 tokens exceed max_supply

    pct.amount = 10;
    pct.band_limit = 1000;
    TS_ASSERT_THROWS_ANYTHING(hand.apply_purchaseCT(addr, pct, Hash::rand()));

    // Buy just 8 tokens
    pct.amount = 8;
    pct.band_limit = 1000;
    hand.apply_purchaseCT(addr, pct, Hash::rand());
    TS_ASSERT_EQUALS(20, contract.get_current_supply());
  }
};
