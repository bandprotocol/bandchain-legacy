#pragma once

#include "inc/essential.h"
#include "store/context.h"

class ContextMap : public Context
{
public:
  ContextMap();

  Object* get(const Hash& key) const final;

  bool check(const Hash& key) const final;

  void add(const Hash& key, std::unique_ptr<Object> obj) final;

private:
  std::unordered_map<Hash, std::unique_ptr<Object>> data;
};
