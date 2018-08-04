#include <array>
#include <chrono>
#include <iostream>
#include <sodium/crypto_hash_sha256.h>
#include <sodium/crypto_sign_ed25519.h>
#include <string>

using namespace std;

template <int SIZE>
struct Hash {
  Hash(const string& hex)
  {
    for (size_t i = 0; i < hex.size(); ++i) {
      unsigned char v = hex[i] <= '9' ? hex[i] - '0' : hex[i] - 'a' + 10;
      if (i % 2 == 0)
        v <<= 4;
      data[i / 2] |= v;
    }
  }

  unsigned char data[SIZE] = {};
};

void sha256(const string& str)
{
  unsigned char out[32];
  crypto_hash_sha256(out, (const unsigned char*)str.c_str(), str.size());
}

bool ed25519_verify(const Hash<64>& sig, const string& data, const Hash<32>& vk)
{
  return crypto_sign_ed25519_verify_detached(sig.data,
                                             (const unsigned char*)data.c_str(),
                                             data.size(), vk.data) == 0;
}

int main()
{

  auto start_sha256 = chrono::system_clock::now();
  for (int i = 0; i < 500'000; ++i) {
    sha256("hello, world!");
    sha256("hello, world?");
  }
  auto end_sha256 = chrono::system_clock::now();
  chrono::duration<double> diff_sha256 = end_sha256 - start_sha256;
  std::cout << "Time to do 1M sha256 is " << diff_sha256.count() << " s\n";

  Hash<64> sig("d53e6dd9dc1c72cded26fde1c9fba01d6ac747123a105cdcf6d183"
               "d6f31f62bffa9"
               "baf9014549a66858ecad302b473f9926089fd024e96fad71c43d76"
               "a42df02");
  Hash<32> vk("be0cba0e63b4ddd47d9866dc0e71b81520c83a20a54323d2350588"
              "db86f0fa85");
  auto start_ed25519 = chrono::system_clock::now();
  for (int i = 0; i < 5'000; ++i) {
    ed25519_verify(sig, "hello, world!", vk);
    ed25519_verify(sig, "hello, world?", vk);
  }
  auto end_ed25519 = chrono::system_clock::now();
  chrono::duration<double> diff_ed25519 = end_ed25519 - start_ed25519;
  std::cout << "Time to do 10k ed25519 is " << diff_ed25519.count() << " s\n";
  return 0;
}
