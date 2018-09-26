// #include <cxxtest/TestSuite.h>

// #include "band/handler.h"
// #include "band/msg.h"
// #include "band/txgen.h"
// #include "crypto/ed25519.h"
// #include "store/account.h"
// #include "store/context_map.h"
// #include "store/contract.h"
// #include "store/revenue.h"
// #include "util/buffer.h"
// #include "util/equation.h"
// #include "util/iban.h"
// #include "util/json.h"
// #include "util/time.h"

// class HandlerTest : public CxxTest::TestSuite
// {
// public:
//   void testMint()
//   {
//     ContextMap ctx;
//     Handler hand(ctx);
//     MintMsg mint;
//     mint.token_key =
//         ContractID::from_hex("0000000000000000000000000000000000000000");
//     mint.value = 10000;
//     AccountID addr = AccountID::rand();
//     hand.apply_mint(addr, mint, Hash::rand(), 0);

//     // Check account balance
//     auto& account = ctx.get<Account>(addr);
//     TS_ASSERT_EQUALS(10000, account[mint.token_key]);
//   }

//   void testInit()
//   {
//     ContextMap ctx;
//     Handler hand(ctx);
//     hand.init("");

//     auto& revenue_base = ctx.get<Revenue>(
//         RevenueID::from_string("RX70 RRRR RRRR RRRR RRRR RRRR RRRR RRRR
//         RRRR"));

//     TS_ASSERT_EQUALS(
//         ContractID::from_string("CX09 CCCC CCCC CCCC CCCC CCCC CCCC CCCC
//         CCCC"), revenue_base.base_token_id);
//     TS_ASSERT_EQUALS(
//         StakeID::from_string("SX87 SSSS SSSS SSSS SSSS SSSS SSSS SSSS SSSS"),
//         revenue_base.stake_id);
//     TS_ASSERT_EQUALS(
//         AccountID::from_string("AX72 AAAA AAAA AAAA AAAA AAAA AAAA AAAA
//         AAAA"), revenue_base.manager);
//     TS_ASSERT_EQUALS(false, revenue_base.is_private);
//     TS_ASSERT_EQUALS(398, revenue_base.base_to_x(398));
//     TS_ASSERT_EQUALS(92839, revenue_base.x_to_base(92839));

//     auto& band_contract = ctx.get<Contract>(ContractID::from_string(
//         "CX09 CCCC CCCC CCCC CCCC CCCC CCCC CCCC CCCC"));
//     TS_ASSERT_EQUALS(
//         ContractID::from_string("CX09 CCCC CCCC CCCC CCCC CCCC CCCC CCCC
//         CCCC"), band_contract.base_contract_id);
//     TS_ASSERT_EQUALS(
//         AccountID::from_string("AX72 AAAA AAAA AAAA AAAA AAAA AAAA AAAA
//         AAAA"), band_contract.beneficiary);
//     TS_ASSERT_EQUALS(
//         RevenueID::from_string("RX70 RRRR RRRR RRRR RRRR RRRR RRRR RRRR
//         RRRR"), band_contract.revenue_id);
//     TS_ASSERT_EQUALS(true, band_contract.is_transferable);
//     TS_ASSERT_EQUALS(false, band_contract.is_discountable);
//     TS_ASSERT_EQUALS(398, band_contract.get_buy_price(398));
//     TS_ASSERT_EQUALS(0, band_contract.get_sell_price(92839));
//   }

//   void testCreateCommunityContract()
//   {
//     ContextMap ctx;
//     Handler hand(ctx);
//     hand.init("");

//     // Create community contract
//     AccountID MrA = AccountID::rand();
//     ContractID contract_id = create_community_contract(hand, MrA);

//     ctx.flush();
//     // Test
//     auto& contract = ctx.get<Contract>(contract_id);
//     TS_ASSERT_EQUALS(
//         ContractID::from_string("CX09 CCCC CCCC CCCC CCCC CCCC CCCC CCCC
//         CCCC"), contract.base_contract_id);
//     TS_ASSERT_EQUALS(MrA, contract.beneficiary);
//     TS_ASSERT_EQUALS(
//         RevenueID::from_string("RX70 RRRR RRRR RRRR RRRR RRRR RRRR RRRR
//         RRRR"), contract.revenue_id);
//     TS_ASSERT_EQUALS(true, contract.is_transferable);
//     TS_ASSERT_EQUALS(false, contract.is_discountable);
//     TS_ASSERT_EQUALS(3582, contract.get_buy_price(398));
//     TS_ASSERT_EQUALS(464195, contract.get_sell_price(92839));
//   }

//   void testCreateRevenue()
//   {
//     ContextMap ctx;
//     Handler hand(ctx);
//     hand.init("");

//     // Create community contract
//     AccountID MrA = AccountID::rand();
//     ContractID contract_id = create_community_contract(hand, MrA);
//     ctx.flush();

//     // Create Revenue from MrA
//     RevenueID revenue_id = create_revenue(hand, MrA, contract_id);
//     ctx.flush();

//     // Test
//     auto& revenue = ctx.get<Revenue>(revenue_id);
//     TS_ASSERT_EQUALS(contract_id, revenue.base_token_id);
//     TS_ASSERT_EQUALS(
//         StakeID::from_string("SX87 SSSS SSSS SSSS SSSS SSSS SSSS SSSS SSSS"),
//         revenue.stake_id);
//     TS_ASSERT_EQUALS(MrA, revenue.manager);
//     TS_ASSERT_EQUALS(true, revenue.is_private);
//     TS_ASSERT_EQUALS(7, revenue.base_to_x(15));
//     TS_ASSERT_EQUALS(14, revenue.x_to_base(7));
//   }

//   void testCreateProductContract()
//   {
//     ContextMap ctx;
//     Handler hand(ctx);
//     hand.init("");

//     // Create community contract
//     AccountID MrA = AccountID::rand();
//     ContractID contract_id = create_community_contract(hand, MrA);
//     ctx.flush();

//     // Create Revenue from MrA
//     RevenueID revenue_id = create_revenue(hand, MrA, contract_id);
//     ctx.flush();

//     // Create Product token based on revenue
//     ContractID product_id = create_product_contract(hand, MrA, revenue_id);
//     ctx.flush();

//     auto& product = ctx.get<Contract>(product_id);
//     TS_ASSERT_EQUALS(contract_id, product.base_contract_id);
//     TS_ASSERT_EQUALS(MrA, product.beneficiary);
//     TS_ASSERT_EQUALS(revenue_id, product.revenue_id);
//     TS_ASSERT_EQUALS(false, product.is_transferable);
//     TS_ASSERT_EQUALS(true, product.is_discountable);
//     TS_ASSERT_EQUALS(39800, product.get_buy_price(398));
//     TS_ASSERT_EQUALS(9375, product.get_sell_price(1875));
//   }

//   void testPurchaseCTAndSellCT()
//   {
//     ContextMap ctx;
//     Handler hand(ctx);
//     hand.init("");

//     AccountID mng1 = AccountID::rand();
//     AccountID fc1 = AccountID::rand();
//     AccountID fc2 = AccountID::rand();

//     ContractID band =
//         ContractID::from_string("CX09 CCCC CCCC CCCC CCCC CCCC CCCC CCCC
//         CCCC");
//     auto account_fc1 = &ctx.get_or_create<Account>(fc1);
//     account_fc1->operator[](band) = 10000;

//     auto account_fc2 = &ctx.get_or_create<Account>(fc2);
//     account_fc2->operator[](band) = 1000;

//     Account* account_mng1 = &ctx.get_or_create<Account>(mng1);

//     // Create simple community contract
//     ContractID contract_id = create_community_contract(hand, mng1);
//     ctx.flush();

//     auto contract = &ctx.get<Contract>(contract_id);

//     // Purchase community tokens
//     PurchaseContractMsg pct;
//     pct.value = 10;
//     pct.contract_id = contract_id;
//     pct.price_limit = 150; // enough to buy

//     hand.apply_purchase_contract(fc1, pct, Hash::rand(), 0);
//     ctx.flush();

//     account_fc1 = &ctx.get_or_create<Account>(fc1);
//     contract = &ctx.get<Contract>(contract_id);
//     account_mng1 = &ctx.get_or_create<Account>(mng1);
//     // Test after purchase ct
//     TS_ASSERT_EQUALS(9910, account_fc1->operator[](band));
//     TS_ASSERT_EQUALS(10, account_fc1->operator[](contract_id));

//     TS_ASSERT_EQUALS(10, contract->circulating_supply);
//     TS_ASSERT_EQUALS(40, account_mng1->operator[](band));

//     pct.value = 20;
//     pct.contract_id = contract_id;
//     pct.price_limit = 160; // not enough to buy

//     TS_ASSERT_THROWS_ANYTHING(
//         hand.apply_purchase_contract(fc2, pct, Hash::rand(), 0));

//     ctx.reset();

//     contract = &ctx.get<Contract>(contract_id);
//     account_mng1 = &ctx.get<Account>(mng1);
//     TS_ASSERT_EQUALS(10, contract->circulating_supply);
//     TS_ASSERT_EQUALS(40, account_mng1->operator[](band));

//     pct.price_limit = 200;
//     hand.apply_purchase_contract(fc2, pct, Hash::rand(), 0);
//     ctx.flush();

//     account_fc1 = &ctx.get<Account>(fc1);
//     account_fc2 = &ctx.get<Account>(fc2);
//     contract = &ctx.get<Contract>(contract_id);
//     account_mng1 = &ctx.get<Account>(mng1);
//     // Test after purchase ct
//     TS_ASSERT_EQUALS(9910, account_fc1->operator[](band));
//     TS_ASSERT_EQUALS(820, account_fc2->operator[](band));
//     TS_ASSERT_EQUALS(10, account_fc1->operator[](contract_id));
//     TS_ASSERT_EQUALS(20, account_fc2->operator[](contract_id));

//     TS_ASSERT_EQUALS(30, contract->circulating_supply);
//     TS_ASSERT_EQUALS(120, account_mng1->operator[](band));

//     SellContractMsg sct;
//     sct.value = 15;
//     sct.contract_id = contract_id;
//     sct.price_limit = 0;

//     TS_ASSERT_THROWS_ANYTHING(
//         hand.apply_sell_contract(fc1, sct, Hash::rand(), 0));

//     ctx.reset();
//     contract = &ctx.get<Contract>(contract_id);
//     account_mng1 = &ctx.get<Account>(mng1);
//     account_fc1 = &ctx.get<Account>(fc1);

//     TS_ASSERT_EQUALS(30, contract->circulating_supply);
//     TS_ASSERT_EQUALS(9910, account_fc1->operator[](band));

//     sct.value = 10;
//     sct.price_limit = 100;

//     TS_ASSERT_THROWS_ANYTHING(
//         hand.apply_sell_contract(fc1, sct, Hash::rand(), 0));

//     ctx.reset();

//     contract = &ctx.get<Contract>(contract_id);
//     account_mng1 = &ctx.get<Account>(mng1);
//     account_fc1 = &ctx.get<Account>(fc1);
//     TS_ASSERT_EQUALS(30, contract->circulating_supply);
//     TS_ASSERT_EQUALS(9910, account_fc1->operator[](band));

//     sct.price_limit = 50;
//     hand.apply_sell_contract(fc1, sct, Hash::rand(), 0);
//     TS_ASSERT_EQUALS(9960, account_fc1->operator[](band));
//     TS_ASSERT_EQUALS(0, account_fc1->operator[](contract_id));

//     TS_ASSERT_EQUALS(20, contract->circulating_supply);
//     TS_ASSERT_EQUALS(120, account_mng1->operator[](band));
//   }

//   void testPurchasePT()
//   {
//     ContextMap ctx;
//     Handler hand(ctx);
//     hand.init("");

//     AccountID mng1 = AccountID::rand();
//     AccountID fc1 = AccountID::rand();
//     AccountID fc2 = AccountID::rand();
//     ContractID band =
//         ContractID::from_string("CX09 CCCC CCCC CCCC CCCC CCCC CCCC CCCC
//         CCCC");
//     ContractID contract_id = create_community_contract(hand, mng1);
//     ctx.flush();

//     // Create Revenue from MrA
//     RevenueID revenue_id = create_revenue(hand, mng1, contract_id);
//     ctx.flush();

//     // Create Product token based on revenue
//     ContractID product_id = create_product_contract(hand, mng1, revenue_id);
//     ctx.flush();

//     auto account_fc1 = &ctx.get_or_create<Account>(fc1);
//     account_fc1->operator[](band) = 10000;

//     // Buy Community token
//     PurchaseContractMsg pct;
//     pct.value = 1000;
//     pct.contract_id = contract_id;
//     pct.price_limit = 10000; // enough to buy

//     hand.apply_purchase_contract(fc1, pct, Hash::rand(), 0);
//     ctx.flush();

//     account_fc1 = &ctx.get_or_create<Account>(fc1);
//     auto com_contract = &ctx.get<Contract>(contract_id);
//     auto account_mng1 = &ctx.get_or_create<Account>(mng1);
//     // Test after purchase ct
//     TS_ASSERT_EQUALS(1000, account_fc1->operator[](band));
//     TS_ASSERT_EQUALS(1000, account_fc1->operator[](contract_id));

//     TS_ASSERT_EQUALS(1000, com_contract->circulating_supply);
//     TS_ASSERT_EQUALS(4000, account_mng1->operator[](band));

//     // Buy Product token
//     pct.value = 5;
//     pct.contract_id = product_id;
//     pct.price_limit = 600; // enough to buy
//     hand.apply_purchase_contract(fc1, pct, Hash::rand(), 0);
//     ctx.flush();

//     // Check
//     account_fc1 = &ctx.get<Account>(fc1);
//     TS_ASSERT_EQUALS(500, account_fc1->operator[](contract_id));
//     TS_ASSERT_EQUALS(5, account_fc1->operator[](product_id));

//     account_mng1 = &ctx.get<Account>(mng1);
//     TS_ASSERT_EQUALS(475, account_mng1->operator[](contract_id));

//     SpendTokenMsg spend_msg;
//     spend_msg.token_key = product_id;
//     spend_msg.value = 3;
//     auto ts = get_ts_from_str("2018-07-01T22:04:59Z"); // tpc = 474
//     TimePeriod tp(3, TimeUnit::Month);
//     hand.apply_spend_token(fc1, spend_msg, Hash::rand(), ts);
//     ctx.flush();

//     account_fc1 = &ctx.get<Account>(fc1);
//     TS_ASSERT_EQUALS(500, account_fc1->operator[](contract_id));
//     TS_ASSERT_EQUALS(2, account_fc1->operator[](product_id));

//     auto revenue = &ctx.get<Revenue>(revenue_id);
//     TS_ASSERT_EQUALS(7, revenue->operator[](474));

//     account_mng1 = &ctx.get<Account>(mng1);
//     TS_ASSERT_EQUALS(4000, account_mng1->operator[](band));
//     TS_ASSERT_EQUALS(475, account_mng1->operator[](contract_id));
//   }

// private:
//   ContractID create_community_contract(Handler& hand, const AccountID& MrA)
//   {
//     CreateContractMsg cc_msg;
//     cc_msg.revenue_id =
//         RevenueID::from_string("RX70 RRRR RRRR RRRR RRRR RRRR RRRR RRRR
//         RRRR");

//     cc_msg.beneficiary = MrA;
//     cc_msg.is_discountable = false;
//     cc_msg.is_transferable = true;
//     cc_msg.buy_curve = Curve(
//         std::make_unique<EqMul>(std::move(std::make_unique<EqConstant>(9)),
//                                 std::move(std::make_unique<EqVar>())));
//     cc_msg.sell_curve = Curve(
//         std::make_unique<EqMul>(std::move(std::make_unique<EqConstant>(5)),
//                                 std::move(std::make_unique<EqVar>())));
//     Hash tx_hash = Hash::rand();
//     ContractID contract_id =
//     ContractID::from_addr(ed25519_vk_to_addr(tx_hash));

//     cc_msg.max_supply = 3000;
//     hand.apply_create_contract(MrA, cc_msg, tx_hash, 0);

//     return contract_id;
//   }

//   RevenueID create_revenue(Handler& hand, const AccountID& MrA,
//                            const ContractID& contract_id)
//   {
//     CreateRevenueMsg cr_msg;
//     cr_msg.manager = MrA;
//     cr_msg.base_token_id = contract_id;
//     cr_msg.is_private = true;
//     cr_msg.curve_to_base = Curve(
//         std::make_unique<EqMul>(std::move(std::make_unique<EqConstant>(2)),
//                                 std::move(std::make_unique<EqVar>())));
//     cr_msg.curve_to_x = Curve(
//         std::make_unique<EqDiv>(std::move(std::make_unique<EqVar>()),
//                                 std::move(std::make_unique<EqConstant>(2))));
//     cr_msg.stake_id =
//         StakeID::from_string("SX87 SSSS SSSS SSSS SSSS SSSS SSSS SSSS SSSS");
//     cr_msg.time_period = TimePeriod(3, TimeUnit::Month);
//     Hash tx_hash = Hash::rand();
//     RevenueID revenue_id = RevenueID::from_addr(ed25519_vk_to_addr(tx_hash));
//     hand.apply_create_revenue(MrA, cr_msg, tx_hash, 0);

//     return revenue_id;
//   }

//   ContractID create_product_contract(Handler& hand, const AccountID& MrA,
//                                      const RevenueID& revenue_id)
//   {
//     CreateContractMsg cc_msg;
//     cc_msg.beneficiary = MrA;
//     cc_msg.is_discountable = true;
//     cc_msg.is_transferable = false;
//     cc_msg.max_supply = 99;
//     cc_msg.revenue_id = revenue_id;
//     cc_msg.buy_curve = Curve(
//         std::make_unique<EqMul>(std::move(std::make_unique<EqVar>()),
//                                 std::move(std::make_unique<EqConstant>(100))));
//     cc_msg.sell_curve = Curve(
//         std::make_unique<EqMul>(std::move(std::make_unique<EqVar>()),
//                                 std::move(std::make_unique<EqConstant>(5))));
//     Hash tx_hash = Hash::rand();
//     ContractID product_id =
//     ContractID::from_addr(ed25519_vk_to_addr(tx_hash));
//     hand.apply_create_contract(MrA, cc_msg, tx_hash, 0);
//     return product_id;
//   }

//   int64_t get_ts_from_str(const char* str)
//   {
//     std::tm time{};
//     std::istringstream stream(str);
//     stream >> std::get_time(&time, "%Y-%m-%dT%H:%M:%S");
//     return timegm(&time);
//   }
// };
