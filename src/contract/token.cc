#include "token.h"

#include "store/global.h"

Token::Token(const Address& token_id, const Address& _base_token_id,
             const Curve& _buy_curve)
    : Contract(token_id)
    , base_token_id(_base_token_id)
    , buy_curve(_buy_curve)
{
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

void Token::buy(uint256_t value)
{
  uint256_t buy_price = buy_curve.apply(VarsSimple(current_supply + value)) -
                        buy_curve.apply(VarsSimple(current_supply));

  auto& base_contract = Global::get().m_ctx->get<Token>(base_token_id);

  base_contract.m_balances[get_sender()] -= buy_price;
  m_balances[get_sender()] += value;
  current_supply += value;
}

void Token::sell(uint256_t value)
{
  uint256_t sell_price = buy_curve.apply(VarsSimple(current_supply)) -
                         buy_curve.apply(VarsSimple(current_supply - value));

  auto& base_contract = Global::get().m_ctx->get<Token>(base_token_id);

  base_contract.m_balances[get_sender()] += sell_price;
  m_balances[get_sender()] -= value;
  current_supply -= value;
}

uint256_t Token::balance(Address address) const
{
  if (auto it = m_balances.find(address); it == m_balances.end())
    return 0;
  else
    return m_balances.at(address);
}

void Token::debug_create() const
{
  DEBUG(log, "token created at {} {}", m_addr, (void*)this);
  DEBUG(log, "Base token id: {}", base_token_id);
  DEBUG(log, "Buy curve: {}", buy_curve);
  for (auto& [addr, val] : m_balances) {
    DEBUG(log, "  {} has {}", addr, val);
  }
}

void Token::debug_save() const
{
  DEBUG(log, "token saved at {} {}", m_addr, (void*)this);
  DEBUG(log, "Supply {}", current_supply);
  for (auto& [addr, val] : m_balances) {
    DEBUG(log, "  {} has {}", addr, val);
  }
}
