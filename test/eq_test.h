#include <cxxtest/TestSuite.h>

#include "inc/essential.h"
#include "util/buffer.h"
#include "util/equation.h"

using eq_ptr = std::unique_ptr<Eq>;

class TempVarseq : public Vars
{
public:
  uint256_t get_x() const { return s; }
  uint256_t get_external_price(const PriceID& key) const { return e; }
  uint256_t get_contract_price(const ContractID& key) const { return c; }

  uint256_t s;
  uint256_t e;
  uint256_t c;
};

class EqTest : public CxxTest::TestSuite
{
public:
  void testEqConstant()
  {
    TempVarseq t;
    EqConstant c(5);
    TS_ASSERT_EQUALS(5, c.apply(t));
  }

  void testEqVar(void)
  {
    TempVarseq t;
    t.s = 20;
    EqVar v;
    TS_ASSERT_EQUALS(20, v.apply(t));
  }

  void testEqSimple()
  {
    TempVarseq t;

    t.s = 20;

    // Create equation s + 5

    // First method
    EqAdd A(std::make_unique<EqVar>(), std::make_unique<EqConstant>(5));
    TS_ASSERT_EQUALS(25, A.apply(t));
    // Second method
    std::unique_ptr<Eq> v = std::make_unique<EqVar>();
    std::unique_ptr<Eq> c = std::make_unique<EqConstant>(5);
    std::unique_ptr<Eq> a1 =
        std::make_unique<EqAdd>(std::move(v), std::move(c));

    // EqConstant c(5);
    TS_ASSERT_EQUALS(25, a1->apply(t));
  }

  void testPolynomial()
  {
    TempVarseq t;
    t.s = 20;
    // x^3 -3x^2 + 2x - 7
    eq_ptr x = std::make_unique<EqVar>();
    eq_ptr c = std::make_unique<EqConstant>(3);
    eq_ptr x3 = std::make_unique<EqExp>(std::move(x), std::move(c));

    x = std::make_unique<EqVar>();
    c = std::make_unique<EqConstant>(2);
    x = std::make_unique<EqExp>(std::move(x), std::move(c));
    c = std::make_unique<EqConstant>(3);
    x = std::make_unique<EqMul>(std::move(c), std::move(x));
    x3 = std::make_unique<EqSub>(std::move(x3), std::move(x));

    x = std::make_unique<EqVar>();
    c = std::make_unique<EqConstant>(2);
    x = std::make_unique<EqMul>(std::move(c), std::move(x));
    x3 = std::make_unique<EqAdd>(std::move(x3), std::move(x));

    c = std::make_unique<EqConstant>(7);
    x3 = std::make_unique<EqSub>(std::move(x3), std::move(c));

    TS_ASSERT_EQUALS(6833, x3->apply(t));

    Buffer buf;

    // Copy constructor and clone()
    Curve curve(std::move(x3));

    Curve curve2 = curve;

    TS_ASSERT_EQUALS(6833, curve2.apply(t));
  }

  // void testPriceSpread()
  // {
  //   // Constant type
  //   PriceSpread pc(SpreadType::Constant, 2);
  //   TS_ASSERT_EQUALS(280, pc.get_sell_price(300, 10));
  //   TS_ASSERT_EQUALS(0, pc.get_sell_price(5, 10));

  //   // Rational type
  //   PriceSpread pcr(SpreadType::Rational, 700000);
  //   TS_ASSERT_EQUALS(210, pcr.get_sell_price(300, 10));
  //   TS_ASSERT_EQUALS(412, pcr.get_sell_price(589, 100));
  // }

  void testOtherVariable()
  {
    TempVarseq t;

    // (s^3) + (p * (c * (s * 10))))
    eq_ptr x = std::make_unique<EqVar>();
    eq_ptr c = std::make_unique<EqConstant>(3);
    eq_ptr x3 = std::make_unique<EqExp>(std::move(x), std::move(c));

    x = std::make_unique<EqVar>();
    c = std::make_unique<EqConstant>(10);
    x = std::make_unique<EqMul>(std::move(x), std::move(c));
    ContractID con = ContractID::rand();
    c = std::make_unique<EqContract>(con);
    x = std::make_unique<EqMul>(std::move(c), std::move(x));
    PriceID pri = PriceID::rand();
    c = std::make_unique<EqPrice>(pri);
    x = std::make_unique<EqMul>(std::move(c), std::move(x));
    x3 = std::make_unique<EqAdd>(std::move(x3), std::move(x));

    Buffer buf;
    x3->dump(buf);
    Curve curve(std::move(x3));
    TS_ASSERT_EQUALS(curve.to_string(),
                     "((x ^ 3) + ({} * ({} * (x * 10))))"_format(pri, con));

    t.s = 20;
    t.c = 3;
    t.e = 30;
    TS_ASSERT_EQUALS(26000, curve.apply(t));
    // TS_ASSERT_EQUALS(6793, curve.apply_sell(t));
    t.s = 123;
    t.c = 65;
    t.e = 30;
    TS_ASSERT_EQUALS(4259367, curve.apply(t));

    // 01 06 08 07 03 03 09 0cc674eb90d776205b5f9e66b5dad7304bf6781e 03 0a
    // 316084bcace1624be775bbfb25270c875e80bce9 03 08 07 0a

    Curve curve2;
    buf >> curve2;
    t.s = 20;
    t.c = 3;
    t.e = 30;
    TS_ASSERT_EQUALS(26000, curve2.apply(t));
    // TS_ASSERT_EQUALS(6793, curve.apply_sell(t));
    t.s = 123;
    t.c = 65;
    t.e = 30;
    TS_ASSERT_EQUALS(4259367, curve2.apply(t));
  }
};
