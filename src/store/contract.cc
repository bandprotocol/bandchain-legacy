#include "contract.h"

#include "store/context.h"
#include "store/global.h"

Address Contract::get_sender()
{
  Address sender = Global::get().sender;
  if (sender.is_empty())
    throw Error("Cannot get transaction sender");

  return sender;
}

void Contract::set_sender() { Global::get().sender = m_addr; }
