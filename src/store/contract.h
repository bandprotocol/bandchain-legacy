#pragma once

#include <enum/enum.h>
#include <functional>
#include <unordered_map>

#include "inc/essential.h"
#include "util/buffer.h"
#include "util/bytes.h"
#include "util/json.h"
#include "util/typeid.h"

BETTER_ENUM(ContractID, uint16_t, Creator = 0, Account = 1, Token = 2,
            Voting = 3, Registry = 4)

class Contract
{
  friend class ContractStaticInit;
  friend class VotingTest;

public:
  virtual ~Contract() {}

  virtual ContractID contract_id() const = 0;
  virtual std::unique_ptr<Contract> clone() const = 0;
  virtual void debug_create() const = 0;
  virtual void debug_save() const = 0;

  void call_buf(Buffer& in_buf, Buffer* out_buf);

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

  // Assert with condition
  void assert_con(bool condition, std::string error_msg) const;

public:
  const Address m_addr;

  static const json& get_abi_interface();

private:
  template <typename T = void, typename... Args>
  static void _callable_params_gen(json& obj)
  {
    if constexpr (std::is_void_v<T>) {
      return;
    } else {
      obj["params"].push_back(TypeID<T>::name);
      _callable_params_gen<Args...>(obj);
    }
  }

  template <typename T = void, typename... Args>
  static void _construct_params_gen(json& obj)
  {
    if constexpr (std::is_void_v<T>) {
      return;
    } else {
      obj["constructor_params"].push_back(TypeID<T>::name);
      _construct_params_gen<Args...>(obj);
    }
  }

  /// TODO: Move this to static level
  template <typename T, typename Ret, typename... Args>
  static void add_callable(ContractID contract_id, uint16_t func_id,
                           const std::string& func_name,
                           Ret (T::*func)(Args...))
  {
    all_functions[contract_id._to_integral()].emplace(
        func_id, [func](void* obj, Buffer& in_buf, Buffer* out_buf) {
          std::tuple<T*, Args...> tup{(T*)obj, in_buf.read<Args>()...};
          std::function<Ret(T*, Args...)> func_cast = func;

          if constexpr (!std::is_void_v<Ret>) {
            if (out_buf) {
              auto result = std::apply(func_cast, tup);
              (*out_buf) << result;
              return;
            }
          }

          std::apply(func_cast, tup);
        });

    json& abi_contract = abi_interface[contract_id._to_string()];
    abi_contract["id"] = contract_id._to_integral();

    json& abi_func = abi_contract[func_name];
    abi_func["opcode"] = func_id;
    abi_func["result"] = TypeID<Ret>::name;
    abi_func["type"] = "action";
    _callable_params_gen<Args...>(abi_func);
  }

  template <typename T, typename Ret, typename... Args>
  static void add_callable(ContractID contract_id, uint16_t func_id,
                           const std::string& func_name,
                           Ret (T::*func)(Args...) const)
  {
    all_functions[contract_id._to_integral()].emplace(
        func_id, [func](void* obj, Buffer& in_buf, Buffer* out_buf) {
          std::tuple<T*, Args...> tup{(T*)obj, in_buf.read<Args>()...};
          std::function<Ret(T*, Args...)> func_cast = func;

          if constexpr (!std::is_void_v<Ret>) {
            if (out_buf) {
              auto result = std::apply(func_cast, tup);
              (*out_buf) << result;
              return;
            }
          }

          std::apply(func_cast, tup);
        });

    json& abi_contract = abi_interface[contract_id._to_string()];
    abi_contract["id"] = contract_id._to_integral();

    json& abi_func = abi_contract[func_name];
    abi_func["opcode"] = func_id;
    abi_func["result"] = TypeID<Ret>::name;
    abi_func["type"] = "query";
    _callable_params_gen<Args...>(abi_func);
  }

  template <typename... Args>
  static void add_constructor(ContractID contract_id)
  {
    json& abi_contract = abi_interface[contract_id._to_string()];

    _construct_params_gen<Args...>(abi_contract);
  }
  using BufferFunc = std::function<void(void*, Buffer&, Buffer*)>;
  using BufferFuncMap = std::unordered_map<uint16_t, BufferFunc>;

  inline static std::array<BufferFuncMap, ContractID::_size()> all_functions;
  inline static json abi_interface;
};

class ContractStaticInit
{
public:
  ContractStaticInit();
};
