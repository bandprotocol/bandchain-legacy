#pragma once

#include "inc/essential.h"
#include "util/buffer.h"
#include "util/bytes.h"
#include "util/endian.h"
#include "util/variable.h"

enum class OpCode : uint16_t {
  Add = 1,
  Sub = 2,
  Mul = 3,
  Div = 4,
  Mod = 5,
  Exp = 6,
  Constant = 7,
  Variable = 8,
  Price = 9,
  Contract = 10,
};

enum class SpreadType : uint8_t {
  Constant = 1,
  Rational = 2,
};

class Eq;

class PriceSpread
{
public:
  PriceSpread() {}
  PriceSpread(SpreadType _spread_type, uint256_t _spread_value)
      : spread_type(_spread_type)
      , spread_value(_spread_value)
  {
  }

  SpreadType get_spread_type() { return spread_type; }
  uint256_t get_spread_value() { return spread_value; }

  friend Buffer& operator>>(Buffer& buf, PriceSpread& price_spread);
  friend Buffer& operator<<(Buffer& buf, const PriceSpread& price_spread);

  uint256_t get_sell_price(uint256_t price, uint256_t value) const;

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
      , price_spread(PriceSpread(SpreadType::Constant, 0))
  {
  }
  Curve(std::unique_ptr<Eq> _equation, const PriceSpread& _price_spread)
      : equation(std::move(_equation))
      , price_spread(_price_spread)
  {
  }

  Curve(Curve&& curve) = default;

  ~Curve();

  friend Buffer& operator>>(Buffer& buf, Curve& curve);
  friend Buffer& operator<<(Buffer& buf, const Curve& curve);

  uint256_t apply_buy(const Vars& vars) const;
  uint256_t apply_sell(const Vars& vars) const;

  PriceSpread get_price_spread() const;
  std::string to_string() const;

private:
  std::unique_ptr<Eq> equation;
  PriceSpread price_spread;
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

template <typename T, OpCode op, char op_char>
class EqBinary : public Eq
{
public:
  EqBinary(std::unique_ptr<Eq> _left, std::unique_ptr<Eq> _right)
      : left(std::move(_left))
      , right(std::move(_right))
  {
  }
  EqBinary(Buffer& buf)
      : left(std::move(Eq::parse(buf)))
      , right(std::move(Eq::parse(buf)))
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
  EqConstant(uint256_t _constant)
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
  EqPrice(const ContextKey& _address)
      : address(_address)
  {
  }
  uint256_t apply(const Vars& vars) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
  std::unique_ptr<Eq> clone() const final;

  ContextKey address;
};

class EqContract : public Eq
{
public:
  EqContract() {}
  EqContract(const ContextKey& _address)
      : address(_address)
  {
  }
  uint256_t apply(const Vars& vars) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
  std::unique_ptr<Eq> clone() const final;

  ContextKey address;
};
