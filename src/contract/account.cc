#include "account.h"

#include "crypto/ed25519.h"
#include "store/context.h"
#include "store/global.h"

Account::Account(const VerifyKey& verify_key)
    : Contract(ed25519_vk_to_addr(verify_key))
    , m_verify_key(verify_key)
    , m_nonce(0)
{
}

Buffer Account::delegate_call(Buffer buf)
{
  auto sig = buf.read<Signature>();
  if (!ed25519_verify(sig, m_verify_key, buf.as_span()))
    throw Error("Invalid Ed25519 signature");

  if (m_nonce != buf.read<uint64_t>())
    throw Error("Invalid nonce");
  ++m_nonce;

  set_sender();

  Buffer ret;
  Global::get().m_ctx->call(buf, &ret);
  return ret;
}
