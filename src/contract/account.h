#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/contract.h"
#include "util/bytes.h"

class Account final : public Contract
{
public:
  Account(const VerifyKey& verify_key);

  Buffer delegate_call(Buffer buf);

  ContractID contract_id() const final { return ContractID::Account; }

  std::unique_ptr<Contract> clone() const final
  {
    return std::make_unique<Account>(*this);
  }

  void debug_create() const final
  {
    DEBUG(log, "account created at {} nonce = {} {}", m_addr, m_nonce,
          (void*)this);
  }

  void debug_save() const final
  {
    DEBUG(log, "account saved at {} nonce = {} {}", m_addr, m_nonce,
          (void*)this);
  }

private:
  const VerifyKey m_verify_key;
  uint64_t m_nonce = 0;
  static inline auto log = logger::get("account");
};
