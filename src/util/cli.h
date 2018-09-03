#pragma once

#include <map>

#include "inc/essential.h"

class CmdArgBase;

class Cmd
{
public:
  Cmd(int argc, char* argv[]);

  std::string help() const;

public:
  std::vector<std::string> args;

  inline static std::map<std::string, CmdArgBase*> cmd_map;
};

class CmdArgBase
{
public:
  CmdArgBase(std::string _name, std::string _desc, char _abbrev = '\0');

  const std::string name;   //< The full name of this cmd flag
  const std::string desc;   //< The description of this flag
  const char abbrev = '\0'; //< One character abbreviation

public:
  /// Parse the incoming text into this cmd arg. Can throw.
  virtual void parse(const std::string& data) = 0;

  /// Return the helpful string representation of what this is.
  std::string help() const;

  /// Whether this cmd is given.
  bool given() { return passed; }

public:
  bool passed = false; //< Whether this cmd arg is passed from cli

protected:
  virtual std::string get_type() const = 0;

private:
  void add_to_cmd(const std::string& key);
};

template <typename T>
class CmdArgT : public CmdArgBase
{
public:
  using CmdArgBase::CmdArgBase;

  /// Get the cmd arg as the templated type.
  T get() { return value; }

  /// Convenient operator to return the content of this cmd arg.
  T operator+() { return value; }

protected:
  T value;
};

template <typename T>
class CmdArg;

template <>
class CmdArg<bool> : public CmdArgT<bool>
{
public:
  using CmdArgT::CmdArgT;

private:
  void parse(const std::string& data) final
  {
    if (data == "")
      value = true;
    else if (data == "true")
      value = true;
    else if (data == "false")
      value = false;
    else
      throw Failure("Invalid bool value. Must be true or false");
  }

  std::string get_type() const final { return "bool"; }
};

template <>
class CmdArg<std::string> : public CmdArgT<std::string>
{
public:
  using CmdArgT::CmdArgT;

private:
  void parse(const std::string& data) final { value = data; }

  std::string get_type() const final { return "string"; }
};
