#include "contract.h"

#include "store/context.h"

Address Contract::get_sender()
{
  Address sender = m_ctx->sender;
  if (sender.is_empty())
    throw Error("Cannot get transaction sender");

  return sender;
}

void Contract::set_sender() { m_ctx->sender = m_addr; }
