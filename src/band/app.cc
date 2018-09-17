#include "app.h"

#include "crypto/ed25519.h"
#include "crypto/sha256.h"
#include "store/account.h"

BandApplication::BandApplication(Context& _ctx)
    : ctx(_ctx)
    , handler(ctx)
    , qry(ctx)
{
}

std::string BandApplication::get_current_app_hash() const
{
  return "";
  // return state.hash().to_raw_string();
}

void BandApplication::init(const std::string& init_state)
{
  // Nothing here yet!
}

std::string BandApplication::query(const std::string& path,
                                   const std::string& data)
{
  /// Forward the request to the query instance.
  return qry.process_query(data);
}

void BandApplication::check(const std::string& msg_raw)
{
  auto msg_view = gsl::make_span(msg_raw);
  auto msg_sz = msg_raw.size();

  if (msg_sz < sizeof(SecretKey))
    throw Error("Message is too small");

  /// Split the raw message into body and signature and chunks
  auto msg_body_raw = msg_view.first(msg_sz - sizeof(SecretKey));
  auto msg_sig_raw = msg_view.last(sizeof(SecretKey));

  /// Build the buffers of those chunks
  Buffer msg_body_buf(msg_body_raw);
  Buffer msg_sig_buf(msg_sig_raw);

  /// Retrieve the header and the signature.
  const auto msg_hdr = msg_body_buf.read<MsgHdr>();
  const auto sig = msg_sig_buf.read_all<Signature>();

  /// Verify the signature with the message body.
  if (!ed25519_verify(sig, msg_hdr.vk, msg_body_raw))
    throw Error("Invalid Ed25519 signature");
}

void BandApplication::apply(const std::string& msg_raw)
{
  auto msg_view = gsl::make_span(msg_raw);
  auto msg_sz = msg_raw.size();

  if (msg_sz < sizeof(SecretKey))
    throw Error("Message is too small");

  /// Split the raw message into body and signature and chunks
  auto msg_body_raw = msg_view.first(msg_sz - sizeof(SecretKey));
  auto msg_sig_raw = msg_view.last(sizeof(SecretKey));

  /// Build the buffers of those chunks
  Buffer msg_body_buf(msg_body_raw);
  Buffer msg_sig_buf(msg_sig_raw);

  /// Retrieve the header and the signature.
  const auto msg_hdr = msg_body_buf.read<MsgHdr>();
  const auto sig = msg_sig_buf.read_all<Signature>();

  /// Verify the signature with the message body.
  if (!ed25519_verify(sig, msg_hdr.vk, msg_body_raw))
    throw Error("Invalid Ed25519 signature");

  /// Pass the message to the handler.
  handler.apply_message(msg_hdr, msg_body_buf, sha256(gsl::make_span(msg_raw)),
                        current_timestamp);
}
