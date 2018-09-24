#pragma once

#include "store/contract.h"

class Creator final : public Contract
{
public:
  Creator();

  void create(Buffer buf);

public:
  void debug_create() const final { DEBUG(log, "create created"); }
  void debug_save() const final { DEBUG(log, "creator saved"); }

  std::unique_ptr<Contract> clone() const final
  {
    return std::make_unique<Creator>(*this);
  }
  static inline auto log = logger::get("creator");
};
