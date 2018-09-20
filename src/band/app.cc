#include "app.h"

#include <boost/scope_exit.hpp>

#include "contract/account.h"
#include "contract/creator.h"
#include "store/contract.h"

BandApplication::BandApplication(Context& _ctx)
    : ctx(_ctx)
{
}

std::string BandApplication::get_current_app_hash() const
{
  // TODO
  return "";
}

void BandApplication::init(const std::string& init_state)
{
  ctx.create<Creator>();
  ctx.flush();
}

std::string BandApplication::query(const std::string& path,
                                   const std::string& data)
{
  // TODO
  return "";
}

void BandApplication::check(const std::string& msg_raw)
{
  // TODO
}

void BandApplication::apply(const std::string& msg_raw)
{
  BOOST_SCOPE_EXIT(&ctx) { ctx.reset(); }
  BOOST_SCOPE_EXIT_END

  Buffer msg_buf(gsl::make_span(msg_raw));
  ctx.call(msg_buf);

  ctx.flush();
}
