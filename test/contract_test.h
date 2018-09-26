// #include <cxxtest/TestSuite.h>

// #include "crypto/sha256.h"
// #include "inc/essential.h"
// #include "store/context_map.h"
// #include "store/contract.h"
// #include "util/bytes.h"
// #include "util/equation.h"
// #include "util/iban.h"

// class TempVarsContract : public Vars
// {
// public:
//   uint256_t get_x() const final { return s; }
//   uint256_t get_external_price(const PriceID& key) const final { return e; }
//   uint256_t get_contract_price(const ContractID& key) const final { return c; }

//   uint256_t s;
//   uint256_t e;
//   uint256_t c;
// };

// class ContractTest : public CxxTest::TestSuite
// {
// public:
//   using eq_ptr = std::unique_ptr<Eq>;

//   void testCreatePC()
//   {
//     ContextMap ctx;

//     // Create equation
//     TempVarsContract t;

//     eq_ptr p = get_default_equation();

//     ContractID contract_id = ContractID::rand();
//     RevenueID revenue_id = RevenueID::rand();
//     AccountID benificiary = AccountID::rand();

//     Curve curve(std::move(p));
//     Curve curve2(get_default_equation());
//     ctx.create<Contract>(contract_id, revenue_id, ContractID{}, curve, curve2,
//                          300, 0, 1, benificiary);

//     auto& contract2 = ctx.get<Contract>(contract_id);

//     TS_ASSERT_EQUALS(revenue_id, contract2.revenue_id);

//     TS_ASSERT_EQUALS(300, contract2.max_supply);
//     TS_ASSERT_EQUALS(false, contract2.is_transferable);
//     TS_ASSERT_EQUALS(true, contract2.is_discountable);
//   }

//   void testBuySellPrice()
//   {
//     ContextMap ctx;

//     TempVarsContract t;

//     ContractID contract_id = ContractID::rand();
//     RevenueID revenue_id = RevenueID::rand();
//     AccountID benificiary = AccountID::rand();

//     eq_ptr p = get_default_equation();
//     Curve buy_curve(std::move(p));

//     p = get_default_equation();
//     eq_ptr s = std::make_unique<EqVar>();
//     eq_ptr c = std::make_unique<EqConstant>(4);
//     s = std::make_unique<EqMul>(std::move(s), std::move(c));
//     p = std::make_unique<EqSub>(std::move(p), std::move(s));

//     Curve sell_curve(std::move(p));

//     ctx.create<Contract>(contract_id, revenue_id, ContractID{}, buy_curve,
//                          sell_curve, 300, 1, 0, benificiary);

//     auto& contract2 = ctx.get<Contract>(contract_id);

//     TS_ASSERT_EQUALS(revenue_id, contract2.revenue_id);

//     TS_ASSERT_EQUALS(300, contract2.max_supply);

//     // t.s = 100;
//     TS_ASSERT_EQUALS(970193, contract2.get_buy_price(100));
//     TS_ASSERT_EQUALS(969793, contract2.get_sell_price(100));
//   }

// private:
//   eq_ptr get_default_equation()
//   {
//     // x^3 -3x^2 + 2x - 7
//     eq_ptr x = std::make_unique<EqVar>();
//     eq_ptr c = std::make_unique<EqConstant>(3);
//     eq_ptr x3 = std::make_unique<EqExp>(std::move(x), std::move(c));

//     x = std::make_unique<EqVar>();
//     c = std::make_unique<EqConstant>(2);
//     x = std::make_unique<EqExp>(std::move(x), std::move(c));
//     c = std::make_unique<EqConstant>(3);
//     x = std::make_unique<EqMul>(std::move(c), std::move(x));
//     x3 = std::make_unique<EqSub>(std::move(x3), std::move(x));

//     x = std::make_unique<EqVar>();
//     c = std::make_unique<EqConstant>(2);
//     x = std::make_unique<EqMul>(std::move(c), std::move(x));
//     x3 = std::make_unique<EqAdd>(std::move(x3), std::move(x));

//     c = std::make_unique<EqConstant>(7);
//     x3 = std::make_unique<EqSub>(std::move(x3), std::move(c));

//     return x3;
//   }
// };
