#include <cxxtest/TestSuite.h>

#include "inc/essential.h"
#include "store/context.h"
#include "store/set.h"
#include "store/storage_map.h"
#include "util/buffer.h"

class SetTest : public CxxTest::TestSuite
{
public:
  void testInsetSet()
  {
    std::unique_ptr<Storage> store = std::make_unique<StorageMap>();
    Context ctx(*store);
    Hash set_key = Hash::rand();
    Global::get().m_ctx = &ctx;

    {
      Set<uint16_t> s{set_key};
      s.insert(5);
      s.insert(uint16_t(72));
      Global::get().flush = true;
    }
    {
      Set<uint16_t> s{set_key};
      TS_ASSERT_EQUALS(2, s.size());

      TS_ASSERT_EQUALS(72, s.get_max());
    }

    {
      Set<uint16_t> s{set_key};
      s.insert(uint16_t(36));
    }
    {
      Set<uint16_t> s{set_key};
      s.erase(uint16_t(72));
    }
    {
      Set<uint16_t> s{set_key};
      TS_ASSERT_EQUALS(36, s.get_max());
      TS_ASSERT_EQUALS(2, s.size());
      TS_ASSERT_EQUALS(false, s.insert(uint16_t(5)));
      TS_ASSERT_EQUALS(2, s.size());
    }
  }
};
