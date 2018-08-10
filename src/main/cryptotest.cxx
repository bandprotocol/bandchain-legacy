#include <chrono>

#include "crypto/ed25519.h"
#include "crypto/sha256.h"

int main()
{
  auto start_sha256 = std::chrono::system_clock::now();
  for (int i = 0; i < 500'000; ++i) {
    sha256("hello, world!");
    sha256("hello, world?");
  }
  auto end_sha256 = std::chrono::system_clock::now();
  std::chrono::duration<double> diff_sha256 = end_sha256 - start_sha256;
  log::info("sha256(\"BAND\") = {}", sha256("BAND").to_string());
  log::info("sha256(\"DNAB\") = {}", sha256("DNAB").to_string());
  log::info("Time to do 1M sha256 is {}s", diff_sha256.count());

  auto sig = Signature::from_hex(
      "d53e6dd9dc1c72cded26fde1c9fba01d6ac747123a105cdcf6d183d6f31f62bffa9baf90"
      "14549a66858ecad302b473f9926089fd024e96fad71c43d76a42df02");

  auto vk = VerifyKey::from_hex(
      "be0cba0e63b4ddd47d9866dc0e71b81520c83a20a54323d2350588db86f0fa85");

  auto start_ed25519 = std::chrono::system_clock::now();
  for (int i = 0; i < 5'000; ++i) {
    ed25519_verify(sig, vk, "hello, world!");
    ed25519_verify(sig, vk, "hello, world?");
  }
  auto end_ed25519 = std::chrono::system_clock::now();
  std::chrono::duration<double> diff_ed25519 = end_ed25519 - start_ed25519;
  log::info("Time to do 10k ed25519 is {}s", diff_ed25519.count());

  auto [verify_key, secret_key] = ed25519_generate_keypair();
  log::info("Randomly generated Ed25519 keypair:");
  log::info("secret_key = {}", secret_key);
  log::info("verify_key = {}", verify_key);
  log::info("address = {}", ed25519_vk_to_addr(verify_key));
  Signature band_sig = ed25519_sign(secret_key, "BAND");
  log::info("sign(\"BAND\") = {}", band_sig);
  log::info("verify(sig, vk, \"BAND\"): {}",
            ed25519_verify(band_sig, verify_key, "BAND"));
  return 0;
}
