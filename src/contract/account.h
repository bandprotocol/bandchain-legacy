#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/contract.h"
#include "store/wrapper.h"
#include "util/bytes.h"

class Account final : public Contract
{
public:
  Account(const Address& address);

  void init(const VerifyKey& verify_key);

  Buffer delegate_call(Buffer buf);

  uint256_t get_nonce() const;

  void debug_create() const final
  {
    DEBUG(log, "account created at {} nonce = {} {}", m_addr, m_nonce.get(),
          (void*)this);
  }

  void debug_save() const final
  {
    DEBUG(log, "account saved at {} nonce = {} {}", m_addr, m_nonce.get(),
          (void*)this);
  }

private:
  Wrapper<VerifyKey> m_verify_key{*this};
  Wrapper<uint64_t> m_nonce{*this};
  static inline auto log = logger::get("account");
};
