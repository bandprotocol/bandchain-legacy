#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/context.h"

class ContextMap : public Context
{
public:
  ContextMap();

private:
  Contract* get_impl(const Address& key) const final;

  void add_impl(std::unique_ptr<Contract> obj) final;

private:
  std::unordered_map<Address, std::unique_ptr<Contract>> data;
};
