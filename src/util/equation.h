// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the LICENSE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#pragma once

#include <enum/enum.h>

#include "inc/essential.h"
#include "util/buffer.h"

/// An OpCode enum encodes the operation at the topmost level of an expression.
/// An expression's serialized value starts with the opcode, followed by zero
/// or more serialized values of its sub-expression(s) or its value(s).
ENUM(OpCode,
     uint16_t,
     Add = 1,      //< Addition of two sub-expressions
     Sub = 2,      //< Subtraction of two sub-expressions
     Mul = 3,      //< Multiplication of two sub-expressions
     Div = 4,      //< Division of two sub-expressions
     Mod = 5,      //< Modulus of two sub-expressions
     Exp = 6,      //< Exponentiation of two sub-expressions
     Constant = 7, //< Raw constant unsigned integer
     Variable = 8  //< Run-time specific variable
);

class Eq;

/// A curve encapsulates a bonding curve that in a smart contract. Under the
/// hood, it contains a unique pointer to an equation instance. In the future,
/// this class will be extended to support more features, including price
/// spread, domain, range, and more.
class Curve
{
public:
  Curve();
  ~Curve();

  /// Create a curve from an equation. The newly created equation take
  /// ownership of the passing value.
  Curve(std::unique_ptr<Eq> equation);

  Curve(const Curve& curve);
  Curve(Curve&& curve) = default;

  Curve& operator=(const Curve& curve);
  Curve& operator=(Curve&& curve) = default;

  friend Buffer& operator>>(Buffer& buf, Curve& curve);
  friend Buffer& operator<<(Buffer& buf, const Curve& curve);

  /// Return the y value at the given x value location
  uint256_t apply(const uint256_t& x_value) const;

  /// Return the prettified string representation of this curve
  std::string to_string() const;

public:
  /// Return a newly created linear curve.
  static Curve linear();

private:
  std::unique_ptr<Eq> equation; //< The underlying equation
};

/// An equation is an interface for mathematical expressions in the BAND
/// universe. See below for all the possible implementations.
class Eq
{
public:
  /// Virtual destructor for pure abstract class
  virtual ~Eq() = default;

  /// Apply the equation with the given x value to obtain the matching y value
  virtual uint256_t apply(const uint256_t& x_value) const = 0;

  /// Return readable representation of this expression
  virtual std::string to_string() const = 0;

  /// Serialize this expression into the given buffer
  virtual void dump(Buffer& buf) const = 0;

  /// Return a deep copy of this expression
  virtual std::unique_ptr<Eq> clone() const = 0;

public:
  /// Static method to consume the provided buffer and build an expression
  static std::unique_ptr<Eq> parse(Buffer& buf);
};

template <typename T, OpCode::_enumerated op, char op_char>
class EqBinary : public Eq
{
public:
  EqBinary(std::unique_ptr<Eq> left_expr, std::unique_ptr<Eq> right_expr);
  EqBinary(Buffer& buf);

private:
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
  std::unique_ptr<Eq> clone() const final;

protected:
  std::unique_ptr<Eq> left_expr;
  std::unique_ptr<Eq> right_expr;
};

class EqAdd : public EqBinary<EqAdd, OpCode::Add, '+'>
{
public:
  using EqBinary<EqAdd, OpCode::Add, '+'>::EqBinary;

private:
  uint256_t apply(const uint256_t& x_value) const final;
};

class EqSub : public EqBinary<EqSub, OpCode::Sub, '-'>
{
public:
  using EqBinary<EqSub, OpCode::Sub, '-'>::EqBinary;

private:
  uint256_t apply(const uint256_t& x_value) const final;
};

class EqMul : public EqBinary<EqMul, OpCode::Mul, '*'>
{
public:
  using EqBinary<EqMul, OpCode::Mul, '*'>::EqBinary;

private:
  uint256_t apply(const uint256_t& x_value) const final;
};

class EqDiv : public EqBinary<EqDiv, OpCode::Div, '/'>
{
public:
  using EqBinary<EqDiv, OpCode::Div, '/'>::EqBinary;

private:
  uint256_t apply(const uint256_t& x_value) const final;
};

class EqMod : public EqBinary<EqMod, OpCode::Mod, '%'>
{
public:
  using EqBinary<EqMod, OpCode::Mod, '%'>::EqBinary;

private:
  uint256_t apply(const uint256_t& x_value) const final;
};

class EqExp : public EqBinary<EqExp, OpCode::Exp, '^'>
{
public:
  using EqBinary<EqExp, OpCode::Exp, '^'>::EqBinary;

private:
  uint256_t apply(const uint256_t& x_value) const final;
};

class EqConstant : public Eq
{
public:
  EqConstant(const uint256_t& _constant);

private:
  uint256_t apply(const uint256_t& x_value) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
  std::unique_ptr<Eq> clone() const final;

private:
  uint256_t constant;
};

class EqVar : public Eq
{
private:
  uint256_t apply(const uint256_t& x_value) const final;
  std::string to_string() const final;
  void dump(Buffer& buf) const final;
  std::unique_ptr<Eq> clone() const final;
};
