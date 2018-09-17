#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/context.h"

class ContextMap : public Context
{
public:
  ContextMap();

private:
  Object* get_impl(const ContextKey& key) const final;

  void add_impl(std::unique_ptr<Object> obj) final;

private:
  std::unordered_map<ContextKey, std::unique_ptr<Object>> data;
};
