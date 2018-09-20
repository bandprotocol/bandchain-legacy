#include "creator.h"

#include <enum/enum.h>

#include "contract/account.h"
#include "contract/token.h"
#include "store/context.h"
#include "util/endian.h"

BETTER_ENUM(ContractID, uint16_t, Unset = 0, Account = 1, Token = 2)

Creator::Creator()
    : Contract(Address())
{
  add_callable(1, &Creator::create);
}

void Creator::create(Buffer buf)
{
  ContractID contract_id(ContractID::Unset);
  buf >> contract_id;

  switch (contract_id) {
    case +ContractID::Account:
      m_ctx->create<Account>(buf.read<VerifyKey>());
      break;
    case +ContractID::Token:
      m_ctx->create<Token>();
      break;
    case +ContractID::Unset:
      throw Error("TODO");
  }
}
