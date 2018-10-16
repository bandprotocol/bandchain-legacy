#pragma once

#include <unordered_map>

#include "crypto/sha256.h"
#include "inc/essential.h"
#include "store/contract.h"
#include "store/storage.h"
#include "util/buffer.h"
#include "util/bytes.h"

class Context
{
public:
  Context(Storage& _store);

  void call(Buffer& in_buf, Buffer* out_buf = nullptr);

  void reset();
  void flush();

  template <typename T>
  T& get(const Address& key)
  {
    if (auto ptr = get_contract_ptr(key); ptr != nullptr)
      return *(ptr->as<T>());
    throw Error("Address {} does not exist", key);
  }

  template <typename T>
  T& create(const Address& key)
  {
    auto raw_data = store.get(sha256(key));
    if (raw_data)
      throw Error("Contract has existed already.");

    auto uniq = std::make_unique<T>(key);
    auto raw = uniq.get();
    cache[key] = std::move(uniq);

    return *raw;
  }

  static inline auto log = logger::get("context");

public:
  Storage& store;

private:
  Contract* get_contract_ptr(const Address& key);

private:
  std::unordered_map<Address, std::unique_ptr<Contract>> cache;
};
