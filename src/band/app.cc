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
  if (msg_size < sizeof(MsgBaseVoid))
    throw Error("Invalid base message size {}", msg_size);

  const auto& msg = *reinterpret_cast<const MsgBaseVoid*>(msg_data.c_str());
  if (msg_size != msg.size)
    throw Error("Inconsistent message size {} vs {}", msg_size, msg.size);

  switch (msg.id()) {
    case MintMsg::ID: {
      const auto& mint_msg = msg.as<MintMsg>();
      check_mint(mint_msg);
      if (dry_run == DryRun::No)
        apply_mint(mint_msg);
    }
    case TxMsg::ID: {
      const auto& tx_msg = msg.as<TxMsg>();
      check_tx(tx_msg);
      if (dry_run == DryRun::No)
        apply_tx(tx_msg);
    }
    default:
      throw Error("Invalid message id {}", msg.msgid);
  }
}

void BandApplication::check_mint(const MintMsg& mint_msg) const
{
  if (state.contains(mint_msg.ident))
    throw Error("Mint ident {} already exists", mint_msg.ident);

  if (mint_msg.value.is_empty())
    throw Error("Mint value must not be zero");
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

  for (const auto& tx_input : tx_msg.inputs()) {
    const auto& tx_input_object = state.find<TxOutput>(tx_input.ident);

    if (!tx_input_object.is_spendable(tx_input.vk))
      throw Error("Ident {} is not spendable by {}", tx_input.ident,
                  tx_input.vk);

    if (!ed25519_verify(tx_input.sig, tx_input.vk, tx_input.ident))
      throw Error("Bad Tx signature");

    total_input_value += tx_input_object.get_value();
  }

  for (const auto& tx_output : tx_msg.outputs()) {
    total_output_value += tx_output.value.as_uint256();
  }

  if (total_input_value != total_output_value)
    throw Error("Tx input value {} and Tx output value {} mismatched",
                total_input_value, total_output_value);
}

void BandApplication::apply_tx(const TxMsg& tx_msg)
{
  for (const auto& tx_input : tx_msg.inputs()) {
    auto& tx_input_object = state.find<TxOutput>(tx_input.ident);
    tx_input_object.spend();
  }

  Hash tx_output_ident = sha256(sha256(tx_msg));
  for (const auto& tx_output : tx_msg.outputs()) {
    tx_output_ident = sha256(tx_output_ident);
    state.add(std::make_unique<TxOutput>(
        tx_output.addr, tx_output.value.as_uint256(), tx_output_ident));
  }
}
