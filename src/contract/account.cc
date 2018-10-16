#include "account.h"

#include "crypto/ed25519.h"
#include "store/context.h"
#include "store/global.h"

Account::Account(const Address& address)
    : Contract(address, ContractID::Account)
{
}

void Account::init(const VerifyKey& verify_key)
{
  m_verify_key.set(verify_key);
  m_nonce.set(0);
}

Buffer Account::delegate_call(Buffer buf)
{
  auto sig = buf.read<Signature>();
  if (!ed25519_verify(sig, m_verify_key.get(), buf.as_span()))
    throw Error("Invalid Ed25519 signature");

  uint64_t new_nonce = buf.read<uint64_t>();
  if (m_nonce.get() >= new_nonce)
    throw Error("Invalid nonce");
  m_nonce.set(new_nonce);

  set_sender();

  Buffer ret;
  Global::get().m_ctx->call(buf, &ret);
  return ret;
}

uint256_t Account::get_nonce() const { return m_nonce.get(); }
