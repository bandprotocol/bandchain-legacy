#include "context.h"

#include "store/global.h"

void Context::call(Buffer& in_buf, Buffer* out_buf)
{
  Address addr = in_buf.read<Address>();
  Contract* contract = get_context_impl(addr);

  if (contract == nullptr)
    throw Error("Contract not found");

  Global::get().m_ctx = this; // TODO
  contract->call_buf(in_buf, out_buf);
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
    add_impl(std::move(obj.second));
  }

  cache.clear();
  Global::get().reset_per_tx();

  DEBUG(log, "============================================================"
             "============================================================");
}
