#include <cxxtest/TestSuite.h>

#include "inc/essential.h"
#include "util/buffer.h"
#include "util/eq.h"

using eq_ptr = std::unique_ptr<Eq>;

class TempVars : public Vars
{
public:
  uint256_t get_value(Variable var) const final
  {
    switch (var) {
      case Variable::Supply:
        return 20;
      case Variable::BNDUSD:
        return 100;
      default:
        return 0;
    }
  }
};

class EqTest : public CxxTest::TestSuite
{
public:
  void testEqConstant()
  {
    TempVars t;
    EqConstant c(5);
    TS_ASSERT_EQUALS(5, c.apply(t));
  }

  void testEqVar(void)
  {
    TempVars t;
    EqVar v(Variable::Supply);
    TS_ASSERT_EQUALS(20, v.apply(t));
  }

  void testEqSimple()
  {
    TempVars t;

    // Create equation s + 5

    // First method
    EqAdd A(std::make_unique<EqVar>(Variable::Supply),
            std::make_unique<EqConstant>(5));
    TS_ASSERT_EQUALS(25, A.apply(t));
    // Second method
    std::unique_ptr<Eq> v = std::make_unique<EqVar>(Variable::Supply);
    std::unique_ptr<Eq> c = std::make_unique<EqConstant>(5);
    std::unique_ptr<Eq> a1 =
        std::make_unique<EqAdd>(std::move(v), std::move(c));

    // EqConstant c(5);
    TS_ASSERT_EQUALS(25, a1->apply(t));
  }

  void testPolynomial()
  {
    TempVars t;

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

    TS_ASSERT_EQUALS(6833, x3->apply(t));

    Buffer buf;
    Curve curve(std::move(x3));
    buf << curve;

    Curve curve2;
    buf >> curve2;
  }
};
