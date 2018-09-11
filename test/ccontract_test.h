#include <cxxtest/TestSuite.h>

#include "crypto/sha256.h"
#include "inc/essential.h"
#include "store/ccontract.h"
#include "store/context_map.h"
#include "util/equation.h"

using eq_ptr = std::unique_ptr<Eq>;

class TempVars : public Vars
{
public:
  uint256_t get_value(Variable var) const final
  {
    switch (var) {
      case Variable::Supply:
        return s;
      case Variable::BNDUSD:
        return 100;
      default:
        return 0;
    }
  }

  uint256_t s;
};

class ComunityContractTest : public CxxTest::TestSuite
{
public:
  void testCreateCC()
  {
    ContextMap ctx;

    // Create equation
    TempVars t;

    t.s = 20;

    // x^3 -3x^2 + 2x - 7
    eq_ptr x = std::make_unique<EqVar>(Variable::Supply);
    eq_ptr c = std::make_unique<EqConstant>(3);
    eq_ptr x3 = std::make_unique<EqExp>(std::move(x), std::move(c));

    x = std::make_unique<EqVar>(Variable::Supply);
    c = std::make_unique<EqConstant>(2);
    x = std::make_unique<EqExp>(std::move(x), std::move(c));
    c = std::make_unique<EqConstant>(3);
    x = std::make_unique<EqMul>(std::move(c), std::move(x));
    x3 = std::make_unique<EqSub>(std::move(x3), std::move(x));

    x = std::make_unique<EqVar>(Variable::Supply);
    c = std::make_unique<EqConstant>(2);
    x = std::make_unique<EqMul>(std::move(c), std::move(x));
    x3 = std::make_unique<EqAdd>(std::move(x3), std::move(x));

    c = std::make_unique<EqConstant>(7);
    x3 = std::make_unique<EqSub>(std::move(x3), std::move(c));

    PriceSpread pc2(SpreadType::Rational, 790000);
    Curve curve(std::move(x3), pc2);

    // Generate random tokenKey
    ContractID key = ContractID::rand();
    CommunityContract contract(ctx, key);
    contract.create(curve);
    contract.set_max_supply(120);

    TS_ASSERT_EQUALS(120, contract.get_max_supply());

    TS_ASSERT_EQUALS(6833, curve.apply_buy(t));
    TS_ASSERT_EQUALS(6833, contract.get_buy_price(20));

    t.s = 4;

    TS_ASSERT_EQUALS(17, contract.get_buy_price(4));

    contract.set_current_supply(27);
    TS_ASSERT_EQUALS(27, contract.get_current_supply());

    // Test with random contract_id that haven't been created yet
    ContractID ct = ContractID::rand();
    CommunityContract co2(ctx, ct);
    TS_ASSERT_THROWS_ANYTHING(co2.get_max_supply());

    // Test get/set current profit
    contract.set_current_profit(77777);
    TS_ASSERT_EQUALS(77777, contract.get_current_profit());

    // Test buy_price/sell_price
    TS_ASSERT_EQUALS(713, contract.get_buy_price(10));
    TS_ASSERT_EQUALS(563, contract.get_sell_price(10));

    x = std::make_unique<EqVar>(Variable::Supply);
    c = std::make_unique<EqConstant>(3);
    x3 = std::make_unique<EqExp>(std::move(x), std::move(c));

    x = std::make_unique<EqVar>(Variable::Supply);
    c = std::make_unique<EqConstant>(2);
    x = std::make_unique<EqExp>(std::move(x), std::move(c));
    c = std::make_unique<EqConstant>(3);
    x = std::make_unique<EqMul>(std::move(c), std::move(x));
    x3 = std::make_unique<EqSub>(std::move(x3), std::move(x));

    x = std::make_unique<EqVar>(Variable::Supply);
    c = std::make_unique<EqConstant>(2);
    x = std::make_unique<EqMul>(std::move(c), std::move(x));
    x3 = std::make_unique<EqAdd>(std::move(x3), std::move(x));

    c = std::make_unique<EqConstant>(7);
    x3 = std::make_unique<EqSub>(std::move(x3), std::move(c));

    PriceSpread pc(SpreadType::Constant, 30);
    Curve curve2(std::move(x3), pc);

    key = ContractID::rand();
    CommunityContract contract2(ctx, key);
    contract2.create(curve2);
    contract2.set_max_supply(120);

    TS_ASSERT_EQUALS(59273, contract2.get_buy_price(40));
    TS_ASSERT_EQUALS(58073, contract2.get_sell_price(40));
  }
};
