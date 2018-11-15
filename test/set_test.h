// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the LICENSE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include <cxxtest/TestSuite.h>

#include "inc/essential.h"
#include "store/context.h"
#include "store/graph_set.h"
#include "store/set.h"
#include "store/storage_map.h"
#include "util/buffer.h"

class SetTest : public CxxTest::TestSuite
{
public:
  void testInsetAndEraseSet()
  {
    std::unique_ptr<Storage> store = std::make_unique<StorageMap>();
    std::unique_ptr<GraphStore> graph = std::make_unique<GraphStoreSet>();
    Context ctx(*store, *graph);
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
      // NOCOMMIT_LOG("================================");
      // s.pre_order(1);
      // NOCOMMIT_LOG("================================");
    }
    {
      Set<uint16_t> s{set_key};
      // NOCOMMIT_LOG("================================");
      // s.pre_order(1);
      // NOCOMMIT_LOG("================================");
      TS_ASSERT_EQUALS(36, s.get_max());
      TS_ASSERT_EQUALS(2, s.size());
      TS_ASSERT_EQUALS(false, s.insert(uint16_t(5)));
      TS_ASSERT_EQUALS(2, s.size());
    }
    {
      Set<uint16_t> s{set_key};
      // NOCOMMIT_LOG("================================");
      // s.pre_order(1);
      // NOCOMMIT_LOG("================================");
      s.insert(6);
      s.insert(90);
      s.insert(2);
      s.insert(22);
      s.insert(54);
      s.insert(40);
      s.erase(36);
      s.insert(10);
      s.erase(10);
      // NOCOMMIT_LOG("================================");
      // s.pre_order(1);
      // NOCOMMIT_LOG("================================");
    }
  }

  void testInsert()
  {
    std::unique_ptr<Storage> store = std::make_unique<StorageMap>();
    std::unique_ptr<GraphStore> graph = std::make_unique<GraphStoreSet>();
    Context ctx(*store, *graph);
    Hash set_key = Hash::rand();
    Global::get().m_ctx = &ctx;

    {
      Set<uint16_t> s{set_key};
      s.insert(10);
      s.insert(20);
      s.insert(30);
      Global::get().flush = true;
    }
    {
      Set<uint16_t> s{set_key};
      // s.insert(30);
      // NOCOMMIT_LOG("Before Add 40");
      s.insert(40);
      // NOCOMMIT_LOG("After Add 40");
      // NOCOMMIT_LOG("================================");
      // s.pre_order(1);
      // NOCOMMIT_LOG("================================");
      TS_ASSERT_EQUALS(4, s.size());
      Global::get().flush = true;
    }

    {
      Set<uint16_t> s{set_key};
      // NOCOMMIT_LOG("================================");
      // s.pre_order(1);
      // NOCOMMIT_LOG("================================");
      s.insert(50);
      // NOCOMMIT_LOG("================================");
      // s.pre_order(1);
      // NOCOMMIT_LOG("================================");
      s.insert(25);
    }
    {
      Set<uint16_t> s{set_key};
      // s.pre_order(1);
      TS_ASSERT_EQUALS(6, s.size());
    }
    {
    }
  }
  void testShapeOfTree()
  {
    std::unique_ptr<Storage> store = std::make_unique<StorageMap>();
    std::unique_ptr<GraphStore> graph = std::make_unique<GraphStoreSet>();
    Context ctx(*store, *graph);
    Hash set_key = Hash::rand();
    Global::get().m_ctx = &ctx;

    {
      Set<uint16_t> s{set_key};
      s.insert(11);
      s.insert(7);
      s.insert(12);
      s.insert(2);
      s.insert(8);
      s.insert(13);
      s.insert(1);
      s.insert(3);
      s.insert(4);
      Global::get().flush = true;
    }
    // {
    //   Set<uint16_t> s{set_key};
    //   NOCOMMIT_LOG("====================================");
    //   s.pre_order(1);
    //   NOCOMMIT_LOG("====================================");
    //   s.erase(12);
    // }
    // {
    //   Set<uint16_t> s{set_key};
    //   NOCOMMIT_LOG("====================================");
    //   s.pre_order(8);
    //   NOCOMMIT_LOG("====================================");
    // }
  }

  void testIterator()
  {
    std::unique_ptr<Storage> store = std::make_unique<StorageMap>();
    std::unique_ptr<GraphStore> graph = std::make_unique<GraphStoreSet>();
    Context ctx(*store, *graph);
    Hash set_key = Hash::rand();
    Global::get().m_ctx = &ctx;

    {
      Set<uint16_t> s{set_key};
      s.insert(11);
      s.insert(7);
      s.insert(12);
      s.insert(2);
      s.insert(8);
      s.insert(13);
      s.insert(1);
      s.insert(3);
      s.insert(4);
      Global::get().flush = true;
    }
    {
      // Set<uint16_t> s{set_key};
      // auto it = s.begin();
      // NOCOMMIT_LOG("Begin {}", *it);
      // while (*it) {
      //   NOCOMMIT_LOG("{}", *it);
      //   ++it;
      // }

      // auto it2 = s.last();
      // NOCOMMIT_LOG("LAst {}", *it2);
      // while (*it2) {
      //   NOCOMMIT_LOG("{}", *it2);
      //   --it2;
      // }
    }
  }
};
