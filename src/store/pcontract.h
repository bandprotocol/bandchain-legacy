#pragma once

#include "inc/essential.h"
#include "store/context.h"
#include "store/varcontext.h"
#include "util/equation.h"

class ProductContract
{
public:
  // Create a product contract view from blockchain context and contractID
  ProductContract(Context& _ctx, const ContractID& _contract_id);

  // Create ContractInfo and save to context
  void create(const Curve& _curve, const ContractID& _contract_id);

  // Get the (readable) equation string of this community contract.
  std::string get_string_equation() const;

  PriceSpread get_price_spread() const;
  // Get current supply
  uint256_t get_current_supply() const;

  // Set current supply
  void set_current_supply(uint256_t value);

  // Get max supply
  uint256_t get_max_supply() const;

  // Set max supply
  void set_max_supply(uint256_t value);

  ContractID get_community_contract();

  uint256_t get_buy_price(uint256_t token_supply, uint256_t ct_price) const;

  uint256_t get_sell_price(uint256_t token_supply, uint256_t ct_price) const;

private:
  struct ContractInfo {
    Curve curve;
    uint256_t max_supply{};
    uint256_t current_supply{};

    ContractID community_contract{};
  };

  // Save ContractInfo to context
  void save(const ContractInfo& info) const;

  // Load ContractInfo from context
  ContractInfo load() const;

  Context& ctx;
  const ContractID& contract_id;

  // Key to context
  const Hash key;

  static inline auto log = logger::get("store/pcontract");
};
