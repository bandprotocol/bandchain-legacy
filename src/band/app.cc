#include "app.h"

#include "crypto/ed25519.h"
#include "crypto/sha256.h"
#include "state/output.h"

std::string BandApplication::get_current_app_hash() const
{
  return state.hash().to_raw_string();
}

void BandApplication::init(const std::string& init_state)
{
  // TODO
}

std::string BandApplication::query(const std::string& path,
                                   const std::string& data) const
{
  // TODO
  return state.to_string();
}

void BandApplication::apply(const std::string& msg_data, DryRun dry_run)
{
  const size_t msg_size = msg_data.size();
  if (msg_size < sizeof(Msg))
    throw std::invalid_argument(
        "Invalid base message size {}"_format(msg_size));

  const Msg& msg = *reinterpret_cast<const Msg*>(msg_data.c_str());
  switch (msg.msg_type) {
#define CASE(TYPE, TYPE_LOWER)                                                 \
  case MsgType::TYPE: {                                                        \
    if (msg_size < sizeof(TYPE##Msg))                                          \
      throw std::invalid_argument(                                             \
          "Invalid static message size {}"_format(msg_size));                  \
    const TYPE##Msg& actual_msg = static_cast<const TYPE##Msg&>(msg);          \
    if (msg_size < actual_msg.size())                                          \
      throw std::invalid_argument(                                             \
          "Invalid dynamic message size {}"_format(msg_size));                 \
    check_##TYPE_LOWER(actual_msg);                                            \
    if (dry_run == DryRun::No)                                                 \
      apply_##TYPE_LOWER(actual_msg);                                          \
    break;                                                                     \
  }

    CASE(Mint, mint)
    CASE(Tx, tx)
#undef CASE
    default:
      throw std::invalid_argument(
          "Unknown message type {}"_format(msg.msg_type));
  }
}

void BandApplication::check_mint(const MintMsg& mint_msg) const
{
  if (state.contains(mint_msg.ident))
    throw std::invalid_argument(
        "Mint ident {} already exists"_format(mint_msg.ident));

  if (mint_msg.value.is_empty())
    throw std::invalid_argument("Mint value must not be zero");
}

void BandApplication::apply_mint(const MintMsg& mint_msg)
{
  state.add(std::make_unique<TxOutput>(
      mint_msg.addr, mint_msg.value.as_uint256(), mint_msg.ident));
}

void BandApplication::check_tx(const TxMsg& tx_msg) const
{
  uint256_t total_input_value = 0;
  uint256_t total_output_value = 0;

  for (size_t idx = 0; idx < tx_msg.input_cnt; ++idx) {
    const auto& tx_input = tx_msg.get_input(idx);
    const auto& tx_input_object = state.find<TxOutput>(tx_input.ident);

    if (!tx_input_object.is_spendable(tx_input.vk))
      throw std::invalid_argument("Ident {} is not spendable by {}"_format(
          tx_input.ident, tx_input.vk));

    if (!ed25519_verify(tx_msg.get_signature(idx), tx_input.vk, tx_msg.hash()))
      throw std::invalid_argument("Bad Tx signature");

    total_input_value += tx_input_object.get_value();
  }

  for (size_t idx = 0; idx < tx_msg.output_cnt; ++idx) {
    const auto& tx_output = tx_msg.get_output(idx);
    total_output_value += tx_output.value.as_uint256();
  }

  if (total_input_value != total_output_value)
    throw std::invalid_argument(
        "Tx input value {} and Tx output value {} mismatched"_format(
            total_input_value, total_output_value));
}

void BandApplication::apply_tx(const TxMsg& tx_msg)
{
  for (size_t idx = 0; idx < tx_msg.input_cnt; ++idx) {
    const auto& tx_input = tx_msg.get_input(idx);
    auto& tx_input_object = state.find<TxOutput>(tx_input.ident);

    tx_input_object.spend();
  }

  Hash tx_output_ident = tx_msg.hash();
  for (size_t idx = 0; idx < tx_msg.output_cnt; ++idx) {
    const auto& tx_output = tx_msg.get_output(idx);

    tx_output_ident = sha256(tx_output_ident);
    state.add(std::make_unique<TxOutput>(
        tx_output.addr, tx_output.value.as_uint256(), tx_output_ident));
  }
}
