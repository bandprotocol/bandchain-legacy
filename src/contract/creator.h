#pragma once

#include "store/contract.h"

class Creator final : public Contract
{
public:
  Creator(const Address& address);

  Address create(Buffer buf);

public:
  void debug_create() const final { DEBUG(log, "create created"); }
  void debug_save() const final { DEBUG(log, "creator saved"); }

  static inline auto log = logger::get("creator");
};
