#include <cxxtest/TestSuite.h>

#include "inc/essential.h"
class OutputTest : public CxxTest::TestSuite
{
public:
  void testFooAddition(void)
  {
    TS_ASSERT(1 + 1 > 1);
    TS_ASSERT_EQUALS(1 + 1, 2);
  }
};
