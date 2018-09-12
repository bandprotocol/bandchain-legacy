#include "store/varcontext.h"

VarsContext::VarsContext(Context& _ctx, uint256_t _supply)
    : ctx(_ctx)
    , supply(_supply)
{
}

uint256_t VarsContext::get_value(Variable var) const
{
  switch (var) {
    case Variable::Supply:
      return supply;
    case Variable::BNDUSD:
      // TODO
      return 10;
    default:
      throw Error("This var isn't supported by system");
  }
}

VarsContextPT::VarsContextPT(Context& _ctx, uint256_t _supply,
                             uint256_t _ct_price)
    : VarsContext::VarsContext(_ctx, _supply)
    , ct_price(_ct_price)
{
}

uint256_t VarsContextPT::get_value(Variable var) const
{
  if (var == Variable::CTPrice)
    return ct_price;
  else
    return VarsContext::get_value(var);
}
