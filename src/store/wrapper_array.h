// #pragma once

// #include <vector>

// #include "inc/essential.h"
// #include "store/global.h"

// template <typename T>
// class WrapperArray
// {
// public:
//   WrapperArray(Contract& contract)
//       : key(sha256(contract.m_addr, contract.increment_then_get()))
//       , flush(contract.flush)
//       , rewritten(false)
//   {
//     // Get value from key and store as vector
//     auto result = Global::get().m_ctx->store.get(key);
//     if (result) {
//       Buffer buf{gsl::make_span(*result)};
//       uint256_t size = buf.read<uint256_t>();
//       data.resize(size);
//       for (auto& member : data) {
//         member = buf.read<T>();
//       }
//     } else {
//       data.clear();
//     }
//   }

//   ~WrapperArray()
//   {
//     if (flush && rewritten) {
//       Buffer buf;
//       buf << data.size();
//       for (auto& member : data) {
//         buf << member;
//       }
//       DEBUG(log, "Saved array at key {}", key);
//       Global::get().m_ctx->store.put(key, buf.to_raw_string());
//     }
//   }

//   // T operator+() const
//   // {
//   //   if (cache)
//   //     return *cache;
//   //   else {
//   //     auto result = Global::get().m_ctx->store.get(key);
//   //     if (result) {
//   //       cache = Buffer::deserialize<T>(*result);
//   //       return *cache;
//   //     } else
//   //       throw Error("Failed to get value");
//   //   }
//   // }

//   // void operator=(T value)
//   // {
//   //   cache = value;
//   //   rewritten = true;
//   // }

//   T& operator[](const uint256_t idx)
//   {
//     if (idx >= data.size())
//       throw Error("Out of range.");
//     rewritten = true;
//     return data[idx];
//   }

//   void resize(const uint256_t count) { data.resize(count); }

// private:
//   const Hash key;
//   const bool& flush;
//   bool rewritten;
//   std::vector<T> data;
//   static inline auto log = logger::get("warray");
// };