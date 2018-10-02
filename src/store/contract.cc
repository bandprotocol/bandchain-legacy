#include "contract.h"

#include "contract/account.h"
#include "contract/creator.h"
#include "contract/tcr.h"
#include "contract/token.h"
#include "contract/voting.h"
#include "store/context.h"
#include "store/global.h"
#include "util/typeid.h"

ContractStaticInit _contract_init;

ContractStaticInit::ContractStaticInit()
{
#define ADD_CALLABLE(cls, method, idx)                                         \
  Contract::add_callable(ContractID::cls, idx, #method, &cls::method);

  ADD_CALLABLE(Creator, create, 1)
  ADD_CALLABLE(Account, delegate_call, 1)
  ADD_CALLABLE(Account, get_nonce, 2)
  ADD_CALLABLE(Token, mint, 1)
  ADD_CALLABLE(Token, transfer, 2)
  ADD_CALLABLE(Token, buy, 3)
  ADD_CALLABLE(Token, sell, 4)
  ADD_CALLABLE(Token, balance, 5)
  ADD_CALLABLE(Token, spot_price, 6)
  ADD_CALLABLE(Token, bulk_price, 7)
  ADD_CALLABLE(Voting, request_voting_power, 1)
  ADD_CALLABLE(Voting, withdraw_voting_power, 2)
  ADD_CALLABLE(Voting, rescue_token, 3)
  ADD_CALLABLE(Voting, commit_vote, 4)
  ADD_CALLABLE(Voting, reveal_vote, 5)
  ADD_CALLABLE(Voting, get_vote_for, 6)
  ADD_CALLABLE(Voting, get_vote_against, 7)
  ADD_CALLABLE(Voting, get_period, 8)
  ADD_CALLABLE(Voting, get_result, 9)
  ADD_CALLABLE(Voting, get_commit_end_time, 10);
  ADD_CALLABLE(Voting, get_reveal_end_time, 11);
  ADD_CALLABLE(Registry, apply, 1)
  ADD_CALLABLE(Registry, deposit, 2)
  ADD_CALLABLE(Registry, withdraw, 3)
  ADD_CALLABLE(Registry, exit, 4)
  ADD_CALLABLE(Registry, challenge, 5)
  ADD_CALLABLE(Registry, update_status, 6)
  ADD_CALLABLE(Registry, claim_reward, 7)
  ADD_CALLABLE(Registry, get_voting_id, 8)
  ADD_CALLABLE(Registry, active_list_length, 9)
  ADD_CALLABLE(Registry, active_list_id_at, 10)
  ADD_CALLABLE(Registry, get_content, 11)
  ADD_CALLABLE(Registry, get_deposit, 12)
  ADD_CALLABLE(Registry, get_active_challenge, 13)
  ADD_CALLABLE(Registry, need_update, 14)
  ADD_CALLABLE(Registry, get_poll_id, 15)
  ADD_CALLABLE(Registry, is_proposal, 16)
  ADD_CALLABLE(Registry, get_app_expire, 17)
  ADD_CALLABLE(Registry, get_list_owner, 18)
  ADD_CALLABLE(Registry, get_challenger_id, 19)

  Contract::add_constructor<Hash>(ContractID::Account);
  Contract::add_constructor<Address, Buffer>(ContractID::Token);
  Contract::add_constructor<Address>(ContractID::Voting);
  Contract::add_constructor<Address, Address>(ContractID::Registry);

#undef ADD_CALLABLE
}

const json& Contract::get_abi_interface() { return abi_interface; }

void Contract::call_buf(Buffer& in_buf, Buffer* out_buf)
{
  auto func_id = in_buf.read<uint16_t>();
  auto& functions = all_functions[contract_id()._to_integral()];

  if (auto it = functions.find(func_id); it != functions.end()) {
    (it->second)(this, in_buf, out_buf);
  } else {
    throw Error("Invalid function ident");
  }
}

Address Contract::get_sender()
{
  Address sender = Global::get().sender;
  if (sender.is_empty())
    throw Error("Cannot get transaction sender");

  return sender;
}

void Contract::set_sender() { Global::get().sender = m_addr; }

void Contract::assert_con(bool condition, std::string error_msg) const
{
  if (!condition)
    throw Error(error_msg);
}
