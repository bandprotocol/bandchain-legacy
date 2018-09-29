#include <iostream>

#include "inc/essential.h"
#include "store/contract.h"

int main()
{
  std::cout << Contract::get_abi_interface().dump(4) << std::endl;
  return 0;
}
