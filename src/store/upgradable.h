#pragma once

#include "inc/essential.h"
#include "util/buffer.h"

class Upgradable
{
public:
  virtual ~Upgradable() {}
  virtual Address get_governance_id() const = 0;
  virtual void upgrade(Buffer buf) = 0;
  virtual std::string to_string(Buffer buf) const = 0;
};

template <typename T>
class UpgradableImpl : public Upgradable
{
public:
  UpgradableImpl(T _params, const Address& _governance_id)
      : params(_params)
      , governance_id(_governance_id)
  {
  }

  Address get_governance_id() const final { return governance_id; }
  void upgrade(Buffer buf) final { buf >> params; }
  std::string to_string(Buffer buf) const final
  {
    T tmp_params;
    buf >> tmp_params;
    return tmp_params.to_string();
  }

protected:
  T params;
  const Address governance_id;
};
