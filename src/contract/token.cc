#include "token.h"

#include "crypto/ed25519.h"
#include "store/global.h"

Token::Token()
    : Contract(ed25519_vk_to_addr(Global::get().tx_hash))
{
  add_callable(1, &Token::mint);
  add_callable(2, &Token::transfer);
}

void Token::mint(uint256_t value)
{
  // TODO
  m_balances[get_sender()] += value;
}

void Token::transfer(Address dest, uint256_t value)
{
  m_balances[get_sender()] -= value;
  m_balances[dest] += value;
}

void Token::debug_create() const
{
  NOCOMMIT_LOG("token created at {} {}", m_addr, (void*)this);
  for (auto& [addr, val] : m_balances) {
    NOCOMMIT_LOG("  {} has {}", addr, val);
  }
}

void Token::debug_save() const
{
  NOCOMMIT_LOG("token saved at {} {}", m_addr, (void*)this);
  for (auto& [addr, val] : m_balances) {
    NOCOMMIT_LOG("  {} has {}", addr, val);
  }
}
