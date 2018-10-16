#include "context.h"

#include "contract/account.h"
#include "contract/creator.h"
#include "contract/governance.h"
#include "contract/stake.h"
#include "contract/tcr.h"
#include "contract/token.h"
#include "contract/voting.h"
#include "crypto/sha256.h"
#include "store/global.h"

Context::Context(Storage& _store)
    : store(_store)
{
}

void Context::call(Buffer& in_buf, Buffer* out_buf)
{
  Address addr = in_buf.read<Address>();
  Contract* contract = get_contract_ptr(addr);

  if (contract == nullptr)
    throw Error("Contract not found");

  contract->call_buf(in_buf, out_buf);
}

Contract* Context::get_contract_ptr(const Address& key)
{
  if (auto it = cache.find(key); it != cache.end())
    return it->second.get();

  auto raw_data = store.get(sha256(key));
  if (!raw_data)
    return nullptr;

  Buffer buf{gsl::make_span(*raw_data)};
  ContractID contract_id(ContractID::Creator);
  buf >> contract_id;

  switch (contract_id) {
    case +ContractID::Creator:
      cache.emplace(key, std::make_unique<Creator>(key));
      break;
    case +ContractID::Account:
      cache.emplace(key, std::make_unique<Account>(key));
      break;
    case +ContractID::Token:
      cache.emplace(key, std::make_unique<Token>(key));
      break;
    // case +ContractID::Voting:
    //   cache.emplace(key, std::make_unique<Voting>(key));
    //   break;
    // case +ContractID::Registry:
    //   cache.emplace(key, std::make_unique<Registry>(key));
    //   break;
    // case +ContractID::Stake:
    //   cache.emplace(key, std::make_unique<Stake>(key));
    //   break;
    // case +ContractID::Governance:
    //   cache.emplace(key, std::make_unique<Governance>(key));
    //   break;
    default:
      throw Error("Contract id {} doesn't support.", contract_id);
  }

  return cache.at(key).get();
}

void Context::reset()
{
  cache.clear();
  Global::get().reset_per_tx();
}

void Context::flush()
{
  for (auto& obj : cache) {
    obj.second->debug_save();
    obj.second->flush = true;
  }

  cache.clear();
  Global::get().reset_per_tx();

  DEBUG(log, "============================================================"
             "============================================================");
}
