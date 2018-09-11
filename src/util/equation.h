#pragma once

#include "inc/essential.h"
#include "util/buffer.h"
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
  Variable = 8
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
  static std::unique_ptr<Eq> parse(Buffer& buf);
};

class EqBinary : public Eq
{
public:
  EqBinary(std::unique_ptr<Eq> _left, std::unique_ptr<Eq> _right);
  EqBinary(Buffer& buf);

protected:
  std::unique_ptr<Eq> left;
  std::unique_ptr<Eq> right;
};

class EqAdd : public EqBinary
{
public:
  using EqBinary::EqBinary;
  uint256_t apply(const Vars& vars) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
};

class EqSub : public EqBinary
{
public:
  using EqBinary::EqBinary;
  uint256_t apply(const Vars& vars) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
};

class EqMul : public EqBinary
{
public:
  using EqBinary::EqBinary;
  uint256_t apply(const Vars& vars) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
};

class EqDiv : public EqBinary
{
public:
  using EqBinary::EqBinary;
  uint256_t apply(const Vars& vars) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
};

class EqMod : public EqBinary
{
public:
  using EqBinary::EqBinary;
  uint256_t apply(const Vars& vars) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
};

class EqExp : public EqBinary
{
public:
  using EqBinary::EqBinary;
  uint256_t apply(const Vars& vars) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
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

private:
  uint256_t constant;
};

class EqVar : public Eq
{
public:
  EqVar(Variable _var)
      : var(_var)
  {
  }
  uint256_t apply(const Vars& vars) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;

private:
  Variable var;
};
