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
    default:
      throw Error("This var isn't supported by system");
  }
}
