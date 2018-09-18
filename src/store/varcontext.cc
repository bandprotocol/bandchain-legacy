#include "store/varcontext.h"

VarsContext::VarsContext(const uint256_t& _x)
    : x(_x)
{
}

uint256_t VarsContext::get_x() const { return x; }

uint256_t VarsContext::get_external_price(const PriceID& key) const
{
  // TODO
  return x;
}

uint256_t VarsContext::get_contract_price(const ContractID& key) const
{
  // TODO
  return x;
}

// VarsContextPT::VarsContextPT(Context& _ctx, uint256_t _supply,
//                              uint256_t _ct_price)
//     : VarsContext::VarsContext(_ctx, _supply)
//     , ct_price(_ct_price)
// {
// }

// uint256_t VarsContextPT::get_value(Variable var) const
// {
//   if (var == Variable::CTPrice)
//     return ct_price;
//   else
//     return VarsContext::get_value(var);
// }
