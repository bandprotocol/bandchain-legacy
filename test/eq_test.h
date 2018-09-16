// #include <cxxtest/TestSuite.h>

// #include "inc/essential.h"
// #include "util/buffer.h"
// #include "util/equation.h"

// using eq_ptr = std::unique_ptr<Eq>;

// class TempVarseq : public Vars
// {
// public:
//   uint256_t get_value(Variable var) const final
//   {
//     switch (var) {
//       case Variable::Supply:
//         return s;
//       case Variable::BNDUSD:
//         return 100;
//       default:
//         return 0;
//     }
//   }

//   uint256_t s;
// };

// class EqTest : public CxxTest::TestSuite
// {
// public:
//   void testEqConstant()
//   {
//     TempVarseq t;
//     EqConstant c(5);
//     TS_ASSERT_EQUALS(5, c.apply(t));
//   }

//   void testEqVar(void)
//   {
//     TempVarseq t;
//     t.s = 20;
//     EqVar v(Variable::Supply);
//     TS_ASSERT_EQUALS(20, v.apply(t));
//   }

//   void testEqSimple()
//   {
//     TempVarseq t;

//     t.s = 20;

//     // Create equation s + 5

//     // First method
//     EqAdd A(std::make_unique<EqVar>(Variable::Supply),
//             std::make_unique<EqConstant>(5));
//     TS_ASSERT_EQUALS(25, A.apply(t));
//     // Second method
//     std::unique_ptr<Eq> v = std::make_unique<EqVar>(Variable::Supply);
//     std::unique_ptr<Eq> c = std::make_unique<EqConstant>(5);
//     std::unique_ptr<Eq> a1 =
//         std::make_unique<EqAdd>(std::move(v), std::move(c));

//     // EqConstant c(5);
//     TS_ASSERT_EQUALS(25, a1->apply(t));
//   }

//   void testPolynomial()
//   {
//     TempVarseq t;
//     t.s = 20;
//     // x^3 -3x^2 + 2x - 7
//     eq_ptr x = std::make_unique<EqVar>(Variable::Supply);
//     eq_ptr c = std::make_unique<EqConstant>(3);
//     eq_ptr x3 = std::make_unique<EqExp>(std::move(x), std::move(c));

//     x = std::make_unique<EqVar>(Variable::Supply);
//     c = std::make_unique<EqConstant>(2);
//     x = std::make_unique<EqExp>(std::move(x), std::move(c));
//     c = std::make_unique<EqConstant>(3);
//     x = std::make_unique<EqMul>(std::move(c), std::move(x));
//     x3 = std::make_unique<EqSub>(std::move(x3), std::move(x));

//     x = std::make_unique<EqVar>(Variable::Supply);
//     c = std::make_unique<EqConstant>(2);
//     x = std::make_unique<EqMul>(std::move(c), std::move(x));
//     x3 = std::make_unique<EqAdd>(std::move(x3), std::move(x));

//     c = std::make_unique<EqConstant>(7);
//     x3 = std::make_unique<EqSub>(std::move(x3), std::move(c));

//     TS_ASSERT_EQUALS(6833, x3->apply(t));

//     Buffer buf;
//     Curve curve(std::move(x3));
//     buf << curve;

//     Curve curve2;
//     buf >> curve2;
//   }

//   void testPriceSpread()
//   {
//     // Constant type
//     PriceSpread pc(SpreadType::Constant, 2);
//     TS_ASSERT_EQUALS(280, pc.get_sell_price(300, 10));
//     TS_ASSERT_EQUALS(0, pc.get_sell_price(5, 10));

//     // Rational type
//     PriceSpread pcr(SpreadType::Rational, 700000);
//     TS_ASSERT_EQUALS(210, pcr.get_sell_price(300, 10));
//     TS_ASSERT_EQUALS(412, pcr.get_sell_price(589, 100));
//   }

//   void testCurve()
//   {
//     TempVarseq t;

//     // x^3 -3x^2 + 2x - 7
//     eq_ptr x = std::make_unique<EqVar>(Variable::Supply);
//     eq_ptr c = std::make_unique<EqConstant>(3);
//     eq_ptr x3 = std::make_unique<EqExp>(std::move(x), std::move(c));

//     x = std::make_unique<EqVar>(Variable::Supply);
//     c = std::make_unique<EqConstant>(2);
//     x = std::make_unique<EqExp>(std::move(x), std::move(c));
//     c = std::make_unique<EqConstant>(3);
//     x = std::make_unique<EqMul>(std::move(c), std::move(x));
//     x3 = std::make_unique<EqSub>(std::move(x3), std::move(x));

//     x = std::make_unique<EqVar>(Variable::Supply);
//     c = std::make_unique<EqConstant>(2);
//     x = std::make_unique<EqMul>(std::move(c), std::move(x));
//     x3 = std::make_unique<EqAdd>(std::move(x3), std::move(x));

//     c = std::make_unique<EqConstant>(7);
//     x3 = std::make_unique<EqSub>(std::move(x3), std::move(c));

//     PriceSpread pc(SpreadType::Constant, 2);
//     Curve curve(std::move(x3), pc);
//     t.s = 20;
//     TS_ASSERT_EQUALS(6833, curve.apply_buy(t));
//     TS_ASSERT_EQUALS(6793, curve.apply_sell(t));
//     t.s = 374;
//     TS_ASSERT_EQUALS(51894737, curve.apply_buy(t));
//     TS_ASSERT_EQUALS(51893989, curve.apply_sell(t));

//     // x^3 -3x^2 + 2x - 7
//     x = std::make_unique<EqVar>(Variable::Supply);
//     c = std::make_unique<EqConstant>(3);
//     x3 = std::make_unique<EqExp>(std::move(x), std::move(c));

//     x = std::make_unique<EqVar>(Variable::Supply);
//     c = std::make_unique<EqConstant>(2);
//     x = std::make_unique<EqExp>(std::move(x), std::move(c));
//     c = std::make_unique<EqConstant>(3);
//     x = std::make_unique<EqMul>(std::move(c), std::move(x));
//     x3 = std::make_unique<EqSub>(std::move(x3), std::move(x));

//     x = std::make_unique<EqVar>(Variable::Supply);
//     c = std::make_unique<EqConstant>(2);
//     x = std::make_unique<EqMul>(std::move(c), std::move(x));
//     x3 = std::make_unique<EqAdd>(std::move(x3), std::move(x));

//     c = std::make_unique<EqConstant>(7);
//     x3 = std::make_unique<EqSub>(std::move(x3), std::move(c));

//     PriceSpread pc2(SpreadType::Rational, 800000);
//     Curve curve2(std::move(x3), pc2);
//     t.s = 20;
//     TS_ASSERT_EQUALS(6833, curve2.apply_buy(t));
//     TS_ASSERT_EQUALS(5466, curve2.apply_sell(t));
//     t.s = 374;
//     TS_ASSERT_EQUALS(51894737, curve2.apply_buy(t));
//     TS_ASSERT_EQUALS(41515789, curve2.apply_sell(t));
//   }
// };
