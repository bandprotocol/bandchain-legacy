#pragma once

#include "store/contract.h"

class Creator final : public Contract
{
public:
  Creator();

  void create(Buffer buf);

public:
  void debug_create() const final { NOCOMMIT_LOG("create created"); }
  void debug_save() const final { NOCOMMIT_LOG("creator saved"); }

  std::unique_ptr<Contract> clone() const final
  {
    return std::make_unique<Creator>(*this);
  }
};
