#pragma once

#include "net/tmapp.h"
#include "state/merkle.h"

class BandApplication : public TendermintApplication
{
public:
  std::string get_name() const final { return "band"; }
  std::string get_version() const final { return "1.0"; }

  /// TODO
  std::string get_current_app_hash() const final;

  /// TODO
  void init(const std::string& init_state) final;

  /// TODO
  std::string query(const std::string& path,
                    const std::string& data) const final;

  /// TODO
  void apply(const std::string& msg_data, DryRun dry_run) final;

private:
  /// TODO
  // void check_mint(const MintMsg& mint_msg) const;
  // void apply_mint(const MintMsg& mint_msg);

  // /// TODO
  // void check_tx(const TxMsg& tx_msg) const;
  // void apply_tx(const TxMsg& tx_msg);

private:
  // MerkleTree state;
};
