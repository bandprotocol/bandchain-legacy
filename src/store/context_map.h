#pragma once

#include "inc/essential.h"
#include "store/context.h"

class ContextMap : public Context
{
public:
  ContextMap();

  Object* get(const ContextKey& key) const final;

  bool check(const ContextKey& key) const final;

  void add(const ContextKey& key, std::unique_ptr<Object> obj) final;

private:
  std::unordered_map<ContextKey, std::unique_ptr<Object>> data;
};
