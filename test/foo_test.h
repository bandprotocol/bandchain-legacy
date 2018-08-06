#include <cxxtest/TestSuite.h>

class FooTestSuite : public CxxTest::TestSuite
{
public:
  void testFooAddition(void)
  {
    TS_ASSERT(1 + 1 > 1);
    TS_ASSERT_EQUALS(1 + 1, 2);
  }
};
