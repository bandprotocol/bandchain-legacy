#pragma once

#include "inc/essential.h"
#include "net/tmapp.h"
#include "store/context.h"

class BandApplication : public TendermintApplication
{
public:
  BandApplication(Context& _ctx);

  std::string get_name() const final { return "band"; }
  std::string get_version() const final { return "latest"; }

  /// Return the current application merkle tree hash.
  std::string get_current_app_hash() const final;

  /// Initialize the blockchain according to the genesis information.
  void init(const std::string& init_state) final;

  /// Query the blockchain information. Data must be a JSON-serialized string
  /// following BAND RPC specification.
  std::string query(const std::string& path, const std::string& data) final;

  /// Check the transaction. For now it does nothing.
  void check(const std::string& msg_raw) final;

  /// Apply the transaction to the blockchain.
  std::string apply(const std::string& msg_raw) final;

private:
  Context& ctx;
  // Handler handler;
  // Query qry;
};
