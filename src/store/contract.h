#pragma once

#include <enum/enum.h>
#include <functional>
#include <unordered_map>

#include "inc/essential.h"
#include "util/buffer.h"
#include "util/bytes.h"
#include "util/endian.h"

class Context;

class Contract
{
public:
  virtual ~Contract() {}
  virtual std::unique_ptr<Contract> clone() const = 0;

  virtual void debug_create() const = 0;
  virtual void debug_save() const = 0;

  void call_buf(Buffer& in_buf, Buffer* out_buf)
  {
    auto func_id = in_buf.read<uint16_t>();
    if (auto it = functions.find(func_id); it != functions.end()) {
      (it->second)(this, in_buf, out_buf);
    } else {
      throw Error("Invalid function ident");
    }
  }

  template <typename T>
  T* as()
  {
    T* result = dynamic_cast<T*>(this);
    if (result == nullptr)
      throw Error("Invalid contract cast");

    return result;
  }

protected:
  Contract(const Address& addr)
      : m_addr(addr)
  {
  }

  /// Get the sender of the current message. Throws if the sender has not
  /// been set.
  Address get_sender();

  /// Set the sender of the current message to this contract.
  void set_sender();

  /// TODO: Move this to static level
  template <typename T, typename Ret, typename... Args>
  void add_callable(const uint16_t func_id, Ret (T::*func)(Args...))
  {
    functions.emplace(
        func_id, [func](void* obj, Buffer& in_buf, Buffer* out_buf) {
          std::tuple<T*, Args...> tup{(T*)obj, in_buf.read<Args>()...};
          std::function<Ret(T*, Args...)> func_cast = func;

          if constexpr (!std::is_void_v<Ret>) {
            if (out_buf) {
              (*out_buf) << std::apply(func_cast, tup);
              return;
            }
          }

          std::apply(func_cast, tup);
        });
  }

public:
  const Address m_addr;

protected:
  std::unordered_map<uint16_t, std::function<void(void*, Buffer&, Buffer*)>>
      functions;
};
