#pragma once

#include "inc/essential.h"
#include "store/contract.h"
#include "store/data.h"
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
class UpgradableContract : public Contract, public Upgradable
{
public:
  UpgradableContract(const Address& address, ContractID _contract_id)
      : Contract(address, _contract_id)
  {
  }

  Address get_governance_id() const final { return +governance_id; }
  void upgrade(Buffer buf) final { params.upgrade(buf); }
  std::string to_string(Buffer buf) const final
  {
    return params.parse_buffer(buf);
  }

protected:
  T params{sha256(m_addr, uint16_t(0), uint16_t(1))};
  Data<Address> governance_id{sha256(m_addr, uint16_t(0), uint16_t(2))};
};
