#include <cxxtest/TestSuite.h>

#include "crypto/sha256.h"
#include "inc/essential.h"
#include "store/context_map.h"
#include "store/pcontract.h"
#include "util/equation.h"

using eq_ptr = std::unique_ptr<Eq>;

namespace
{
eq_ptr get_default_equation()
{
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

  return std::move(x3);
}

} // namespace

class TempVarspc : public Vars
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

class ProductContractTest : public CxxTest::TestSuite
{
public:
  void testCreatePC()
  {
    ContextMap ctx;

    // Create equation
    TempVarspc t;

    eq_ptr p = get_default_equation();
    ContractID contract_id = ContractID::rand();
    ContractID community_contract_id = ContractID::rand();

    Curve curve(std::move(p), {SpreadType::Constant, 0});
    ProductContract contract(ctx, contract_id);
    contract.create(curve, community_contract_id);

    TS_ASSERT_EQUALS(community_contract_id, contract.get_community_contract());

    TS_ASSERT_EQUALS(0, contract.get_max_supply());
    contract.set_max_supply(25);
    TS_ASSERT_EQUALS(25, contract.get_max_supply());
  }
};
