#include "util/equation.h"

#include "util/endian.h"

namespace
{
uint256_t pow(uint256_t l, uint256_t r)
{
  uint256_t v = 1;
  for (uint256_t i = 0; i < r; ++i) {
    v *= l;
  }
  return v;
}

} // namespace

Curve::~Curve() {}

uint256_t Curve::apply_buy(const Vars& vars) const
{
  if (!equation)
    throw Error("Equation hasn't been set");
  return equation->apply(vars);
}

uint256_t Curve::apply_sell(const Vars& vars) const
{
  if (!equation)
    throw Error("Equation hasn't been set");

  uint256_t price_buy = apply_buy(vars);
  return price_spread.get_sell_price(price_buy,
                                     vars.get_value(Variable::Supply));
}

PriceSpread Curve::get_price_spread() const { return price_spread; }

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
  buf << curve.price_spread;
  return buf;
}

Buffer& operator>>(Buffer& buf, Curve& curve)
{
  curve.equation = std::move(Eq::parse(buf));
  buf >> curve.price_spread;
  return buf;
}

uint256_t PriceSpread::get_sell_price(uint256_t price, uint256_t supply) const
{
  // Check type constant
  switch (spread_type) {
    case SpreadType::Constant: {
      uint256_t dif = spread_value * supply;
      if (price > dif)
        return price - dif;
      else
        return 0;
    }
    case SpreadType::Rational:
      return spread_value * price / PriceSpread::ratio;
    default:
      throw Error("This price spread type {} doesn't match any type in system",
                  (uint8_t)spread_type);
  }
}

Buffer& operator<<(Buffer& buf, const PriceSpread& price_spread)
{
  buf << price_spread.spread_type << price_spread.spread_value;
  return buf;
}

Buffer& operator>>(Buffer& buf, PriceSpread& price_spread)
{
  buf >> price_spread.spread_type >> price_spread.spread_value;
  return buf;
}

std::unique_ptr<Eq> Eq::parse(Buffer& buf)
{
  OpCode opcode;
  buf >> opcode;
  switch (opcode) {
    case OpCode::Add:
      return std::make_unique<EqAdd>(buf);
    case OpCode::Sub:
      return std::make_unique<EqSub>(buf);
    case OpCode::Mul:
      return std::make_unique<EqMul>(buf);
    case OpCode::Div:
      return std::make_unique<EqDiv>(buf);
    case OpCode::Mod:
      return std::make_unique<EqMod>(buf);
    case OpCode::Exp:
      return std::make_unique<EqExp>(buf);
    case OpCode::Constant: {
      uint256_t constant;
      buf >> constant;
      return std::make_unique<EqConstant>(constant);
    }
    case OpCode::Variable: {
      Variable var;
      buf >> var;
      return std::make_unique<EqVar>(var);
    }
  }
  throw Error("Opcode doesn't match any OpCode");
}
EqBinary::EqBinary(std::unique_ptr<Eq> _left, std::unique_ptr<Eq> _right)
    : left(std::move(_left))
    , right(std::move(_right))
{
}

EqBinary::EqBinary(Buffer& buf)
    : left(std::move(Eq::parse(buf)))
    , right(std::move(Eq::parse(buf)))
{
}

std::string EqAdd::to_string() const
{
  return "({} + {})"_format(*left, *right);
}
std::string EqSub::to_string() const
{
  return "({} - {})"_format(*left, *right);
}
std::string EqMul::to_string() const
{
  return "({} * {})"_format(*left, *right);
}
std::string EqDiv::to_string() const
{
  return "({} / {})"_format(*left, *right);
}
std::string EqMod::to_string() const
{
  return "({} % {})"_format(*left, *right);
}
std::string EqExp::to_string() const
{
  return "({} ^ {})"_format(*left, *right);
}
std::string EqConstant::to_string() const { return "{}"_format(constant); }

std::string EqVar::to_string() const
{
  // TODO
  return "Variable";
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

uint256_t EqVar::apply(const Vars& vars) const { return vars.get_value(var); }

void EqAdd::dump(Buffer& buf) const
{
  buf << OpCode::Add;
  left->dump(buf);
  right->dump(buf);
}

void EqSub::dump(Buffer& buf) const
{
  buf << OpCode::Sub;
  left->dump(buf);
  right->dump(buf);
}

void EqMul::dump(Buffer& buf) const
{
  buf << OpCode::Mul;
  left->dump(buf);
  right->dump(buf);
}

void EqDiv::dump(Buffer& buf) const
{
  buf << OpCode::Div;
  left->dump(buf);
  right->dump(buf);
}

void EqMod::dump(Buffer& buf) const
{
  buf << OpCode::Mod;
  left->dump(buf);
  right->dump(buf);
}

void EqExp::dump(Buffer& buf) const
{
  buf << OpCode::Exp;
  left->dump(buf);
  right->dump(buf);
}

void EqConstant::dump(Buffer& buf) const
{
  buf << OpCode::Constant;
  buf << constant;
}

void EqVar::dump(Buffer& buf) const
{
  buf << OpCode::Variable;
  buf << var;
}