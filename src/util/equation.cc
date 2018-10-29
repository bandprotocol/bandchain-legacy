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

#include "util/equation.h"

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

Curve::Curve()
{
}

Curve::Curve(const Curve& curve)
    : equation(curve.equation->clone())
{
}

Curve::Curve(std::unique_ptr<Eq> _equation)
    : equation(std::move(_equation))
{
}

Curve::~Curve()
{
}

Curve& Curve::operator=(const Curve& curve)
{
  equation = curve.equation->clone();
  return *this;
}

uint256_t Curve::apply(const uint256_t& x_value) const
{
  if (!equation)
    throw Error("Curve::apply: Equation does not exist");

  return equation->apply(x_value);
}

std::string Curve::to_string() const
{
  if (!equation)
    return "[NULL_CURVE]";

  return equation->to_string();
}

Buffer& operator<<(Buffer& buf, const Curve& curve)
{
  if (!curve.equation)
    throw Failure("Buffer<<(Curve): Equation does not exist");

  curve.equation->dump(buf);
  return buf;
}

Buffer& operator>>(Buffer& buf, Curve& curve)
{
  curve.equation = Eq::parse(buf);
  return buf;
}

std::unique_ptr<Eq> Eq::parse(Buffer& buf)
{
  OpCode opcode = buf.read<OpCode>();

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
    case +OpCode::Constant:
      return std::make_unique<EqConstant>(buf.read<uint256_t>());
    case +OpCode::Variable:
      return std::make_unique<EqVar>();
  }

  throw Failure("Eq::parse: Invalid Opcode {}", opcode._to_integral());
}

template <typename T, OpCode::_enumerated op, char op_char>
EqBinary<T, op, op_char>::EqBinary(std::unique_ptr<Eq> _left_expr,
                                   std::unique_ptr<Eq> _right_expr)
    : left_expr(std::move(_left_expr))
    , right_expr(std::move(_right_expr))
{
}

template <typename T, OpCode::_enumerated op, char op_char>
EqBinary<T, op, op_char>::EqBinary(Buffer& buf)
    : left_expr(Eq::parse(buf))
    , right_expr(Eq::parse(buf))
{
}

template <typename T, OpCode::_enumerated op, char op_char>
std::unique_ptr<Eq> EqBinary<T, op, op_char>::clone() const
{
  return std::make_unique<T>(left_expr->clone(), right_expr->clone());
}

template <typename T, OpCode::_enumerated op, char op_char>
std::string EqBinary<T, op, op_char>::to_string() const
{
  return "({} {} {})"_format(*left_expr, op_char, *right_expr);
}

template <typename T, OpCode::_enumerated op, char op_char>
void EqBinary<T, op, op_char>::dump(Buffer& buf) const
{
  buf << op;
  left_expr->dump(buf);
  right_expr->dump(buf);
}

std::string EqConstant::to_string() const
{
  return "{}"_format(constant);
}

std::string EqVar::to_string() const
{
  return "X";
}

uint256_t EqAdd::apply(const uint256_t& x_value) const
{
  return left_expr->apply(x_value) + right_expr->apply(x_value);
}

uint256_t EqSub::apply(const uint256_t& x_value) const
{
  return left_expr->apply(x_value) - right_expr->apply(x_value);
}

uint256_t EqMul::apply(const uint256_t& x_value) const
{
  return left_expr->apply(x_value) * right_expr->apply(x_value);
}

uint256_t EqDiv::apply(const uint256_t& x_value) const
{
  return left_expr->apply(x_value) / right_expr->apply(x_value);
}

uint256_t EqMod::apply(const uint256_t& x_value) const
{
  return left_expr->apply(x_value) % right_expr->apply(x_value);
}

uint256_t EqExp::apply(const uint256_t& x_value) const
{
  uint256_t l = left_expr->apply(x_value);
  uint256_t r = right_expr->apply(x_value);
  return pow(l, r);
}

EqConstant::EqConstant(const uint256_t& _constant)
    : constant(_constant)
{
}

uint256_t EqConstant::apply(const uint256_t& x_value) const
{
  return constant;
}

uint256_t EqVar::apply(const uint256_t& x_value) const
{
  return x_value;
}

std::unique_ptr<Eq> EqConstant::clone() const
{
  return std::make_unique<EqConstant>(constant);
}

std::unique_ptr<Eq> EqVar::clone() const
{
  return std::make_unique<EqVar>();
}

void EqConstant::dump(Buffer& buf) const
{
  buf << OpCode::Constant;
  buf << constant;
}

void EqVar::dump(Buffer& buf) const
{
  buf << OpCode::Variable;
}
