#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/contract.h"
#include "util/bytes.h"

class Account final : public Contract
{
public:
  Account(const VerifyKey& verify_key);

  void delegate_call(Signature sig, Buffer buf);

  virtual void debug_create() const final
  {
    NOCOMMIT_LOG("account created at {} nonce = {} {}", m_addr, m_nonce,
                 (void*)this);
  }
  virtual void debug_save() const final
  {
    NOCOMMIT_LOG("account saved at {} nonce = {} {}", m_addr, m_nonce,
                 (void*)this);
  }

  std::unique_ptr<Contract> clone() const final
  {
    return std::make_unique<Account>(*this);
  }

private:
  const VerifyKey m_verify_key;
  uint64_t m_nonce = 0;
};
