#include "contract.h"

#include "contract/account.h"
#include "contract/creator.h"
#include "contract/token.h"
#include "contract/voting.h"
#include "store/context.h"
#include "store/global.h"

ContractStaticInit _contract_init;

ContractStaticInit::ContractStaticInit()
{
#define ADD_CALLABLE(cls, method, idx)                                         \
  Contract::add_callable(ContractID::cls, idx, #method, &cls::method);

  ADD_CALLABLE(Creator, create, 1)
  ADD_CALLABLE(Account, delegate_call, 1)
  ADD_CALLABLE(Token, mint, 1)
  ADD_CALLABLE(Token, transfer, 2)
  ADD_CALLABLE(Token, buy, 3)
  ADD_CALLABLE(Voting, request_voting_power, 1)
  ADD_CALLABLE(Voting, withdraw_voting_power, 2)
  ADD_CALLABLE(Voting, rescue_token, 3)
  ADD_CALLABLE(Voting, commit_vote, 4)
  ADD_CALLABLE(Voting, reveal_vote, 5)

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
