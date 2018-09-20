#include "context.h"

void Context::call(Buffer& buf)
{
  Address addr = buf.read<Address>();
  Contract* contract = get_context_impl(addr);

  if (contract == nullptr)
    throw Error("Contract not found");

  Contract::m_ctx = this; // TODO
  contract->call_buf(buf);
}

void Context::reset()
{
  cache.clear();
  sender = Address();
}

void Context::flush()
{
  for (auto& obj : cache) {
    obj.second->debug_save();
    add_impl(std::move(obj.second));
  }
  cache.clear();
  sender = Address();

  DEBUG(log, "============================================================"
             "============================================================");
}
