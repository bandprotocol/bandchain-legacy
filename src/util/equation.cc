#include "util/equation.h"

#include "util/endian.h"

namespace
{
uint256_t pow(const uint256_t& l, const uint256_t& r)
{
  uint256_t v = 1;
  for (uint256_t i = 0; i < r; ++i) {
    v *= l;
  }
  return v;
}

} // namespace

Curve::Curve(const Curve& _curve)
    : equation(_curve.equation->clone())
{
}

Curve::~Curve() {}

Curve& Curve::operator=(const Curve& _curve)
{
  equation = _curve.equation->clone();
  return *this;
}

uint256_t Curve::apply(const Vars& vars) const
{
  if (!equation)
    throw Error("Equation hasn't been set");
  return equation->apply(vars);
}

std::string Curve::to_string() const
{
  if (equation)
    return equation->to_string();
  else
    return "Null equation";
}

Buffer& operator<<(Buffer& buf, const Curve& curve)
{
  if (!curve.equation)
    throw Failure("Dump on null equation");
  curve.equation->dump(buf);
  return buf;
}

Buffer& operator>>(Buffer& buf, Curve& curve)
{
  curve.equation = Eq::parse(buf);
  return buf;
}

// uint256_t PriceSpread::get_sell_price(const uint256_t& price,
//                                       const uint256_t& supply) const
// {
//   // Check type constant
//   switch (spread_type) {
//     case +SpreadType::Constant: {
//       uint256_t dif = spread_value * supply;
//       if (price > dif)
//         return price - dif;
//       else
//         return 0;
//     }
//     case +SpreadType::Rational:
//       return spread_value * price / PriceSpread::ratio;
//     case +SpreadType::Unset:
//       throw Error("Invalid price spread type");
//   }
// }

// Buffer& operator<<(Buffer& buf, const PriceSpread& price_spread)
// {
//   buf << price_spread.spread_type << price_spread.spread_value;
//   return buf;
// }

// Buffer& operator>>(Buffer& buf, PriceSpread& price_spread)
// {
//   buf >> price_spread.spread_type >> price_spread.spread_value;
//   return buf;
// }

std::unique_ptr<Eq> Eq::parse(Buffer& buf)
{
  OpCode opcode(OpCode::Unset);

  buf >> opcode;

  switch (opcode) {
    case +OpCode::Add:
      return std::make_unique<EqAdd>(buf);
    case +OpCode::Sub:
      return std::make_unique<EqSub>(buf);
    case +OpCode::Mul:
      return std::make_unique<EqMul>(buf);
    case +OpCode::Div:
      return std::make_unique<EqDiv>(buf);
    case +OpCode::Mod:
      return std::make_unique<EqMod>(buf);
    case +OpCode::Exp:
      return std::make_unique<EqExp>(buf);
    case +OpCode::Constant: {
      uint256_t constant;
      buf >> constant;
      return std::make_unique<EqConstant>(constant);
    }
    case +OpCode::Variable:
      return std::make_unique<EqVar>();
    case +OpCode::Contract: {
      auto eq_contract = std::make_unique<EqContract>();
      buf >> eq_contract->contract_id;
      return std::move(eq_contract);
    }
    case +OpCode::Price: {
      auto eq_price = std::make_unique<EqPrice>();
      buf >> eq_price->price_id;
      return std::move(eq_price);
    }
    case +OpCode::Unset:
      throw Error("Unexpected Opcode");
  }

  throw Failure("Invalid Opcode");
}
std::string EqConstant::to_string() const { return "{}"_format(constant); }

std::string EqVar::to_string() const
{
  // TODO
  return "x";
}

std::string EqPrice::to_string() const
{
  // TODO
  return price_id.to_string();
}

std::string EqContract::to_string() const
{
  // TODO
  return contract_id.to_string();
}

uint256_t EqAdd::apply(const Vars& vars) const
{
  return left->apply(vars) + right->apply(vars);
}

uint256_t EqSub::apply(const Vars& vars) const
{
  return left->apply(vars) - right->apply(vars);
}

uint256_t EqMul::apply(const Vars& vars) const
{
  return left->apply(vars) * right->apply(vars);
}

uint256_t EqDiv::apply(const Vars& vars) const
{
  return left->apply(vars) / right->apply(vars);
}

uint256_t EqMod::apply(const Vars& vars) const
{
  return left->apply(vars) % right->apply(vars);
}

uint256_t EqExp::apply(const Vars& vars) const
{
  uint256_t l = left->apply(vars);
  uint256_t r = right->apply(vars);
  return pow(l, r);
}

uint256_t EqConstant::apply(const Vars& vars) const { return constant; }

uint256_t EqVar::apply(const Vars& vars) const { return vars.get_x(); }

uint256_t EqPrice::apply(const Vars& vars) const
{
  return vars.get_external_price(price_id);
}

uint256_t EqContract::apply(const Vars& vars) const
{
  return vars.get_contract_price(contract_id);
}

std::unique_ptr<Eq> EqConstant::clone() const
{
  return std::make_unique<EqConstant>(constant);
}

std::unique_ptr<Eq> EqVar::clone() const { return std::make_unique<EqVar>(); }

std::unique_ptr<Eq> EqPrice::clone() const
{
  return std::make_unique<EqPrice>(price_id);
}

std::unique_ptr<Eq> EqContract::clone() const
{
  return std::make_unique<EqContract>(contract_id);
}

void EqConstant::dump(Buffer& buf) const
{
  buf << OpCode::Constant;
  buf << constant;
}

void EqVar::dump(Buffer& buf) const { buf << OpCode::Variable; }

void EqPrice::dump(Buffer& buf) const
{
  buf << OpCode::Price;
  buf << price_id;
}

void EqContract::dump(Buffer& buf) const
{
  buf << OpCode::Contract;
  buf << contract_id;
}
