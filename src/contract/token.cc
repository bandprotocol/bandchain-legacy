#include "token.h"

#include "store/global.h"

Token::Token(const Address& token_id)
    : Contract(token_id, ContractID::Token)
{
}

void Token::init(const Address& _base_token_id, const Curve& _buy_curve)
{
  base_token_id.set(_base_token_id);
  buy_curve.set(_buy_curve);
  current_supply.set(0);
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
  uint256_t buy_price =
      buy_curve.get().apply(VarsSimple(current_supply.get() + value)) -
      buy_curve.get().apply(VarsSimple(current_supply.get()));

  auto& base_contract = Global::get().m_ctx->get<Token>(base_token_id.get());

  base_contract.m_balances[get_sender()] -= buy_price;
  m_balances[get_sender()] += value;
  current_supply.set(current_supply.get() + value);
}

void Token::sell(uint256_t value)
{
  uint256_t sell_price =
      buy_curve.get().apply(VarsSimple(current_supply.get())) -
      buy_curve.get().apply(VarsSimple(current_supply.get() - value));

  auto& base_contract = Global::get().m_ctx->get<Token>(base_token_id.get());

  base_contract.m_balances[get_sender()] += sell_price;
  m_balances[get_sender()] -= value;
  current_supply.set(current_supply.get() - value);
}

uint256_t Token::balance(Address address) const
{
  auto data = m_balances.try_at(address);
  if (data)
    return *data;
  else
    return 0;
}

uint256_t Token::spot_price() const
{
  return buy_curve.get().apply(VarsSimple(current_supply.get() + 1)) -
         buy_curve.get().apply(VarsSimple(current_supply.get()));
}

uint256_t Token::bulk_price(uint256_t value) const
{
  return (buy_curve.get().apply(VarsSimple(current_supply.get() + value)) -
          buy_curve.get().apply(VarsSimple(current_supply.get()))) /
         value;
}

void Token::debug_create() const
{
  DEBUG(log, "token created at {} {}", m_addr, (void*)this);
  DEBUG(log, "Base token id: {}", base_token_id.get());
  DEBUG(log, "Buy curve: {}", buy_curve.get());
}

void Token::debug_save() const
{
  DEBUG(log, "token saved at {} {}", m_addr, (void*)this);
  DEBUG(log, "Supply {}", current_supply.get());
}
