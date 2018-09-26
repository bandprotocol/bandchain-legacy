#pragma once

#include "inc/essential.h"
#include "store/context.h"
#include "util/bytes.h"

class Global
{
public:
  Context* m_ctx = nullptr;
  Address sender{};
  uint64_t block_time{};
  Address block_proposer{};
  Hash tx_hash{};

  static Global& get()
  {
    if (!global)
      global = std::make_unique<Global>();
    return *global;
  }

  void reset_per_tx()
  {
    sender = Address();
    tx_hash = Hash();
  }

private:
  inline static std::unique_ptr<Global> global;
};
