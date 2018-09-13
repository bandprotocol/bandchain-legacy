// #include <cxxtest/TestSuite.h>

// #include "band/handler.h"
// #include "band/msg.h"
// #include "band/txgen.h"
// #include "crypto/ed25519.h"
// #include "store/account.h"
// #include "store/context_map.h"
// #include "store/pcontract.h"
// #include "util/buffer.h"
// #include "util/equation.h"
// #include "util/iban.h"
// #include "util/json.h"

// class HandlerTest : public CxxTest::TestSuite
// {
// public:
//   void testMint()
//   {
//     ContextMap ctx;
//     Handler hand(ctx);
//     MintMsg mint;
//     mint.token_key =
//         TokenKey::from_hex("0000000000000000000000000000000000000000");
//     mint.value = 10000;

//     Address addr = Address::rand();
//     hand.apply_mint(addr, mint, Hash::rand());

//     // Check account balance
//     Account account(ctx, addr);
//     TS_ASSERT_EQUALS(10000, account.get_band_balance());

//     TS_ASSERT_EQUALS(10000, account.get_balance(mint.token_key));
//   }

//   void testPurchaseCTAndSellCT()
//   {
//     ContextMap ctx;
//     Handler hand(ctx);

//     Address addr = Address::rand();

//     // Mint band token to tester
//     MintMsg mint;
//     mint.token_key =
//         TokenKey::from_hex("0000000000000000000000000000000000000000");
//     mint.value = 10000;
//     hand.apply_mint(addr, mint, Hash::rand());

//     // Create new community contract
//     CreateCCMsg create;
//     std::string m =
//         R"foo({"expressions": ["ADD", "ADD", "MUL", "1", "EXP", "Supply",
//         "2", "MUL", "2", "Supply", "0"],
//                 "max_supply": "20",
//                 "spread_type": "1",
//                 "spread_value": "4"})foo";

//     json j = json::parse(m);

//     // Create body msg of create_msg from  param "expression"
//     std::string pa = txgen::process_createCC(j);

//     // pa store real 00010002.... cannot read
//     Buffer buf;
//     buf << gsl::make_span(pa);

//     buf >> create;
//     Hash tx_hash = Hash::rand();
//     ContractID contract_id = tx_hash.prefix<ContractID::Size>();
//     hand.apply_createCC(addr, create, tx_hash);

//     // Purchase token
//     // Create view community contract
//     CommunityContract contract(ctx, contract_id);
//     PurchaseCTMsg pct;
//     pct.value = 10;
//     pct.contract_id = contract_id;
//     pct.band_limit = 150; // enough to buy

//     hand.apply_purchaseCT(addr, pct, Hash::rand());

//     // Test after purchase ct
//     Account account(ctx, addr);
//     TS_ASSERT_EQUALS(9880, account.get_band_balance());
//     TS_ASSERT_EQUALS(10, account.get_balance(contract_id));

//     TS_ASSERT_EQUALS(10, contract.get_current_supply());
//     TS_ASSERT_EQUALS(40, contract.get_current_profit());

//     // Another tester
//     Address addr2 = Address::rand();

//     // Mint band token to tester2
//     mint.token_key =
//         TokenKey::from_hex("0000000000000000000000000000000000000000");
//     mint.value = 100;
//     hand.apply_mint(addr2, mint, Hash::rand());

//     pct.value = 2;
//     pct.contract_id = contract_id;
//     pct.band_limit = 10; // not enough to buy

//     TS_ASSERT_THROWS_ANYTHING(hand.apply_purchaseCT(addr2, pct,
//     Hash::rand())); TS_ASSERT_EQUALS(40, contract.get_current_profit());

//     Account account2(ctx, addr2);
//     TS_ASSERT_EQUALS(100, account2.get_band_balance());
//     TS_ASSERT_EQUALS(0, account2.get_balance(contract_id));

//     TS_ASSERT_EQUALS(10, contract.get_current_supply());

//     pct.band_limit = 100; // enought to buy;
//     hand.apply_purchaseCT(addr2, pct, Hash::rand());

//     TS_ASSERT_EQUALS(52, account2.get_band_balance());
//     TS_ASSERT_EQUALS(2, account2.get_balance(contract_id));
//     TS_ASSERT_EQUALS(12, contract.get_current_supply());
//     TS_ASSERT_EQUALS(48, contract.get_current_profit());

//     // Buy 10 tokens exceed max_supply

//     pct.value = 10;
//     pct.band_limit = 1000;
//     TS_ASSERT_THROWS_ANYTHING(hand.apply_purchaseCT(addr, pct,
//     Hash::rand())); TS_ASSERT_EQUALS(48, contract.get_current_profit());

//     // Buy just 8 tokens
//     pct.value = 8;
//     pct.band_limit = 1000;
//     hand.apply_purchaseCT(addr, pct, Hash::rand());
//     TS_ASSERT_EQUALS(9608, account.get_band_balance());
//     TS_ASSERT_EQUALS(20, contract.get_current_supply());
//     TS_ASSERT_EQUALS(80, contract.get_current_profit());

//     // Tester 1 sell token
//     SellCTMsg sct;
//     sct.band_limit = 1000;
//     sct.value = 10;
//     sct.contract_id = contract_id;
//     TS_ASSERT_THROWS_ANYTHING(hand.apply_sellCT(addr, sct, Hash::rand()));

//     sct.band_limit = 200;
//     hand.apply_sellCT(addr, sct, Hash::rand());

//     // Check balance
//     TS_ASSERT_EQUALS(9888, account.get_band_balance());
//     TS_ASSERT_EQUALS(10, contract.get_current_supply());
//     TS_ASSERT_EQUALS(80, contract.get_current_profit());

//     TS_ASSERT_THROWS_ANYTHING(hand.apply_sellCT(addr, sct, Hash::rand()));

//     sct.band_limit = 0;

//     TS_ASSERT_THROWS_ANYTHING(hand.apply_sellCT(addr, sct, Hash::rand()));

//     sct.value = 8;
//     hand.apply_sellCT(addr, sct, Hash::rand());
//     TS_ASSERT_EQUALS(9968, account.get_band_balance());
//     TS_ASSERT_EQUALS(2, contract.get_current_supply());
//     TS_ASSERT_EQUALS(80, contract.get_current_profit());

//     pct.band_limit = 20;
//     pct.value = 2;

//     hand.apply_purchaseCT(addr2, pct, Hash::rand());
//     TS_ASSERT_EQUALS(36, account2.get_band_balance());
//     TS_ASSERT_EQUALS(4, account2.get_balance(contract_id));
//     TS_ASSERT_EQUALS(4, contract.get_current_supply());
//     TS_ASSERT_EQUALS(88, contract.get_current_profit());
//   }

//   void testCreatePC()
//   {
//     ContextMap ctx;
//     Handler hand(ctx);

//     Address addr = Address::rand();
//     Account account(ctx, addr);

//     // create_pc.community_contract_id = token_key;

//     CreatePCMsg create_pc;
//     std::string m =
//         R"foo({"expressions": ["ADD", "ADD", "MUL", "1", "EXP", "Supply",
//         "2", "MUL", "2", "Supply", "0"],
//                 "max_supply": "20",
//                 "spread_type": "1",
//                 "spread_value": "4",
//                 "community_contract_id": "BX56 DCF2 D2NN MAJ4 N75J PK48 FLTX
//                 S9UJ JG6S"})foo";

//     json j = json::parse(m);

//     // Create body msg of createPC_msg from  param "expression"
//     std::string pa = txgen::process_createPC(j);

//     // pa store real 00010002.... cannot read
//     Buffer buf;
//     buf << gsl::make_span(pa);
//     buf >> create_pc;

//     Hash tx_hash = Hash::rand();
//     ContractID contract_id = tx_hash.prefix<ContractID::Size>();
//     hand.apply_createPC(addr, create_pc, tx_hash);

//     ProductContract contract(ctx, contract_id);

//     TS_ASSERT_EQUALS(
//         IBAN("BX56 DCF2 D2NN MAJ4 N75J PK48 FLTX S9UJ JG6S", IBANType::Token)
//             .as_addr(),
//         contract.get_community_contract());

//     // Create another view
//     ProductContract contract2(ctx, contract_id);
//     TS_ASSERT_EQUALS(20, contract.get_max_supply());
//   }
// };
