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

#include <cxxopts/cxxopts.hpp>
#include <nonstd/optional.hpp>
#include <vector>

#include "inc/essential.h"

class CmdArgBase;

/// Cmd is the main entry point of all programs in this project. The class is
/// responsible for parsing argv[]/argc using cxxopts library and fill in
/// all the CmdArg<...> that the program has. There must only be one Cmd object
/// that live throught the program lifetime. Cmd also adds two extra flags:
/// "v,verbose" and "h,help" for all binaries.
class Cmd
{
public:
  /// Create the Cmd object and parse command line arguments. Also take the
  /// description of this program in order to display useful help message.
  Cmd(const char* desc, int argc, char* argv[]);

private:
  cxxopts::Options options;

public:
  /// The list of all CmdArgs available in the current running binary.
  static inline std::vector<CmdArgBase*> args;

  /// The result of parsing the command line arguments using cxxopts.
  static inline nonstd::optional<cxxopts::ParseResult> parseResult;
};

/// CmdArgBase provides the common interface among all of the CmdArg types that
/// do not rely on the type information.
class CmdArgBase
{
public:
  /// Contruct the CmdArg. This is supposed to be created at the global scope
  /// a source file / or in main() before the creation of Cmd. While being
  /// constructed, CmdArg registers itself to the global register to wihch Cmd
  /// will use to figure out how many CmdArgs the program has.
  CmdArgBase(const std::string& opts,
             const std::string& desc,
             const std::string& defaultStr = "");

  /// Return true iff this CmdArg is provided by the user. Note that this may
  /// return false even when the CmdArg has default value.
  bool given() const;

  /// Add this CmdArg into the global registry. Need to be implemented by
  /// the underlying type since there is no type information here.
  virtual void addOption(cxxopts::Options& options) const = 0;

protected:
  const std::string opts;       //< The opts argument (short and long)
  const std::string desc;       //< The text description of this CmdArg
  const std::string defaultStr; //< Default value, or empty string if not exist
  const std::string key;        //< cxxopts lookup key
};

/// Specific implementation of CmdArg that is templated on the underlying type
/// of the CmdArg itself. Support all the types that cxxopts supports. See its
/// documentation for more details.
template <typename T>
class CmdArg : public CmdArgBase
{
public:
  using CmdArgBase::CmdArgBase;

  /// Return the value of this command line arg. Throw on error.
  T get() const
  {
    return Cmd::parseResult->operator[](key).template as<T>();
  }

  /// Shorthand function that behave exactly similar to get().
  T operator+() const
  {
    return get();
  }

private:
  void addOption(cxxopts::Options& options) const final
  {
    auto value = cxxopts::value<T>();
    if (!defaultStr.empty())
      value->default_value(defaultStr);

    options.add_options()(opts, desc, value);
  }

private:
  nonstd::optional<T> value;
};
