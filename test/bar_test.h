#include <cxxtest/TestSuite.h>

class BarTestSuite : public CxxTest::TestSuite
{
public:
  void testBarAddition(void)
  {
    TS_ASSERT(1 + 1 > 1);
    TS_ASSERT_EQUALS(1 + 1, 2);
  }
};
