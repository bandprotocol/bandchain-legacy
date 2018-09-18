#pragma once

#include <enum/enum.h>

#include "inc/essential.h"
#include "util/buffer.h"
#include "util/bytes.h"
#include "util/endian.h"
#include "util/variable.h"

BETTER_ENUM(OpCode, uint16_t, Unset = 0, Add = 1, Sub = 2, Mul = 3, Div = 4,
            Mod = 5, Exp = 6, Constant = 7, Variable = 8, Price = 9,
            Contract = 10)

BETTER_ENUM(SpreadType, uint8_t, Unset = 0, Constant = 1, Rational = 2)

class Eq;

class PriceSpread
{
public:
  PriceSpread()
      : spread_type(SpreadType::Unset)
  {
  }

  PriceSpread(SpreadType _spread_type, uint256_t _spread_value)
      : spread_type(_spread_type)
      , spread_value(_spread_value)
  {
  }

  SpreadType get_spread_type() const { return spread_type; }
  uint256_t get_spread_value() const { return spread_value; }

  friend Buffer& operator>>(Buffer& buf, PriceSpread& price_spread);
  friend Buffer& operator<<(Buffer& buf, const PriceSpread& price_spread);

  uint256_t get_sell_price(const uint256_t& price,
                           const uint256_t& value) const;

private:
  SpreadType spread_type;
  uint256_t spread_value;
  inline static const uint256_t ratio = 1'000'000;
};

class Curve
{
public:
  Curve() {}
  Curve(const Curve& _curve);
  Curve(std::unique_ptr<Eq> _equation)
      : equation(std::move(_equation))
  {
  }

  Curve(Curve&& curve) = default;

  ~Curve();

  friend Buffer& operator>>(Buffer& buf, Curve& curve);
  friend Buffer& operator<<(Buffer& buf, const Curve& curve);

  uint256_t apply(const Vars& vars) const;

  std::string to_string() const;

private:
  std::unique_ptr<Eq> equation;
};

class Eq
{
public:
  virtual ~Eq() {}
  virtual uint256_t apply(const Vars& vars) const = 0;
  virtual std::string to_string() const = 0;
  virtual void dump(Buffer& buf) const = 0;
  virtual std::unique_ptr<Eq> clone() const = 0;
  static std::unique_ptr<Eq> parse(Buffer& buf);
};

template <typename T, OpCode::_enumerated op, char op_char>
class EqBinary : public Eq
{
public:
  EqBinary(std::unique_ptr<Eq> _left, std::unique_ptr<Eq> _right)
      : left(std::move(_left))
      , right(std::move(_right))
  {
  }
  EqBinary(Buffer& buf)
      : left(Eq::parse(buf))
      , right(Eq::parse(buf))
  {
  }

  std::unique_ptr<Eq> clone() const final
  {
    return std::make_unique<T>(left->clone(), right->clone());
  }

  std::string to_string() const final
  {
    return "({} {} {})"_format(*left, op_char, *right);
  }

  void dump(Buffer& buf) const final
  {
    buf << op;
    left->dump(buf);
    right->dump(buf);
  }

protected:
  std::unique_ptr<Eq> left;
  std::unique_ptr<Eq> right;
};

class EqAdd : public EqBinary<EqAdd, OpCode::Add, '+'>
{
public:
  using EqBinary<EqAdd, OpCode::Add, '+'>::EqBinary;
  uint256_t apply(const Vars& vars) const final;
};

class EqSub : public EqBinary<EqSub, OpCode::Sub, '-'>
{
public:
  using EqBinary<EqSub, OpCode::Sub, '-'>::EqBinary;
  uint256_t apply(const Vars& vars) const final;
};

class EqMul : public EqBinary<EqMul, OpCode::Mul, '*'>
{
public:
  using EqBinary<EqMul, OpCode::Mul, '*'>::EqBinary;
  uint256_t apply(const Vars& vars) const final;
};

class EqDiv : public EqBinary<EqDiv, OpCode::Div, '/'>
{
public:
  using EqBinary<EqDiv, OpCode::Div, '/'>::EqBinary;
  uint256_t apply(const Vars& vars) const final;
};

class EqMod : public EqBinary<EqMod, OpCode::Mod, '%'>
{
public:
  using EqBinary<EqMod, OpCode::Mod, '%'>::EqBinary;
  uint256_t apply(const Vars& vars) const final;
};

class EqExp : public EqBinary<EqExp, OpCode::Exp, '^'>
{
public:
  using EqBinary<EqExp, OpCode::Exp, '^'>::EqBinary;
  uint256_t apply(const Vars& vars) const final;
};

class EqConstant : public Eq
{
public:
  EqConstant(const uint256_t& _constant)
      : constant(_constant)
  {
  }
  uint256_t apply(const Vars& vars) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
  std::unique_ptr<Eq> clone() const final;

private:
  uint256_t constant;
};

class EqVar : public Eq
{
public:
  EqVar() {}
  uint256_t apply(const Vars& vars) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
  std::unique_ptr<Eq> clone() const final;
};

class EqPrice : public Eq
{
public:
  EqPrice() {}
  EqPrice(const PriceID& _price_id)
      : price_id(_price_id)
  {
  }
  uint256_t apply(const Vars& vars) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
  std::unique_ptr<Eq> clone() const final;

  PriceID price_id;
};

class EqContract : public Eq
{
public:
  EqContract() {}
  EqContract(const ContractID& _contract_id)
      : contract_id(_contract_id)
  {
  }
  uint256_t apply(const Vars& vars) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
  std::unique_ptr<Eq> clone() const final;

  ContractID contract_id;
};
