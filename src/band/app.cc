#include "app.h"

#include <boost/scope_exit.hpp>

#include "contract/account.h"
#include "contract/creator.h"
#include "contract/token.h"
#include "crypto/sha256.h"
#include "store/contract.h"
#include "store/global.h"
#include "util/cli.h"
#include "util/equation.h"

CmdArg<bool> fake_time("fake_time", "use tx time as global time", 'f');

BandApplication::BandApplication(Context& _ctx)
    : ctx(_ctx)
    , use_fake_time(+fake_time)
{
  Global::get().m_ctx = &ctx;
}

std::string BandApplication::get_current_app_hash() const
{
  // TODO
  return "";
}

void BandApplication::init(const std::string& init_state)
{
  ctx.create<Creator>(Address{});
  Address band = Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
  Curve linear = Curve(std::make_unique<EqVar>());
  auto& token = ctx.create<Token>(band);
  token.init(band, linear);
  ctx.flush();
}

std::string BandApplication::query(const std::string& path,
                                   const std::string& data)
{
  BOOST_SCOPE_EXIT(&ctx) { ctx.reset(); }
  BOOST_SCOPE_EXIT_END

  Buffer msg_buf(gsl::make_span(data));
  uint64_t ts = msg_buf.read<uint64_t>();
  (void)ts;

  Buffer result;
  ctx.call(msg_buf, &result);

  return result.to_raw_string();
}

void BandApplication::check(const std::string& msg_raw)
{
  // TODO
}

std::string BandApplication::apply(const std::string& msg_raw)
{
  BOOST_SCOPE_EXIT(&ctx) { ctx.reset(); }
  BOOST_SCOPE_EXIT_END

  Buffer msg_buf(gsl::make_span(msg_raw));
  Global::get().tx_hash = sha256(gsl::make_span(msg_raw));
  uint64_t ts = msg_buf.read<uint64_t>();
  // Mock
  if (use_fake_time)
    Global::get().block_time = ts;
  // (void)ts;

  Buffer result;
  ctx.call(msg_buf, &result);
  ctx.flush();

  return result.to_raw_string();
}
