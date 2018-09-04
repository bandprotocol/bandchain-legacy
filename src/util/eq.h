#pragma once

#include "inc/essential.h"
#include "util/buffer.h"

enum class Variable : uint8_t {
  Supply = 1,
  BNDUSD = 2,
  BNDTHB = 3,
};

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
class Vars
{
public:
  virtual ~Vars() {}
  virtual uint256_t get_value(Variable var) const = 0;
};

class Eq;

class Curve
{
public:
  Curve() {}
  Curve(std::unique_ptr<Eq> _equation)
      : equation(std::move(_equation))
  {
  }
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
