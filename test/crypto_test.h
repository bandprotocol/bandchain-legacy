#include <cxxtest/TestSuite.h>

#include "inc/essential.h"
class CryptoTest : public CxxTest::TestSuite
{
public:
  void testBarAddition(void)
  {
    TS_ASSERT(1 + 1 > 1);
    TS_ASSERT_EQUALS(1 + 1, 2);
  }
};
