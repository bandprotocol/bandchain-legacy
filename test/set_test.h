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
#include "store/contract.h"
#include "store/graph_set.h"
#include "store/set.h"
#include "store/storage_map.h"
#include "util/buffer.h"
#include "util/string.h"

class TestContract final : public Contract
{
public:
  using Contract::Contract;

  static constexpr char KeyPrefix[] = "test/";

  void init() {}
  SET(uint16_t, s)
};

class SetTest : public CxxTest::TestSuite
{
public:
  void testInsetAndEraseSet()
  {
    StorageMap storage;
    storage.switchToApply();
    storage.create<TestContract>(Ident{"set"});
    {
      storage.switchToApply();
      auto& setContract = storage.load<TestContract>(Ident{"set"});
      setContract.s.insert(5);
      setContract.s.insert(uint16_t(72));
      storage.flush();
    }
    {
      storage.switchToApply();
      auto& setContract = storage.load<TestContract>(Ident{"set"});

      TS_ASSERT_EQUALS(2, setContract.s.size());
      TS_ASSERT_EQUALS(72, setContract.s.get_max());
    }
    {
      storage.switchToApply();
      auto& setContract = storage.load<TestContract>(Ident{"set"});
      setContract.s.insert(uint16_t(36));
      storage.flush();
    }
    {
      storage.switchToApply();
      auto& setContract = storage.load<TestContract>(Ident{"set"});
      setContract.s.erase(uint16_t(72));
      storage.flush();
      // NOCOMMIT_LOG("================================");
      // s.pre_order(1);
      // NOCOMMIT_LOG("================================");
    }
    {
      storage.switchToApply();
      auto& setContract = storage.load<TestContract>(Ident{"set"});
      // NOCOMMIT_LOG("================================");
      // s.pre_order(1);
      // NOCOMMIT_LOG("================================");
      TS_ASSERT_EQUALS(36, setContract.s.get_max());
      TS_ASSERT_EQUALS(2, setContract.s.size());
      TS_ASSERT_EQUALS(false, setContract.s.insert(uint16_t(5)));
      TS_ASSERT_EQUALS(2, setContract.s.size());
    }
    {
      storage.switchToApply();
      auto& setContract = storage.load<TestContract>(Ident{"set"});
      // NOCOMMIT_LOG("================================");
      // s.pre_order(1);
      // NOCOMMIT_LOG("================================");
      setContract.s.insert(6);
      setContract.s.insert(90);
      setContract.s.insert(2);
      setContract.s.insert(22);
      setContract.s.insert(54);
      setContract.s.insert(40);
      setContract.s.erase(36);
      setContract.s.insert(10);
      setContract.s.erase(10);
      // storage.flush();
      // NOCOMMIT_LOG("================================");
      // setContract.s.pre_order(1);
      // NOCOMMIT_LOG("================================");
    }
  }

  void testInsert()
  {
    StorageMap storage;
    storage.switchToApply();
    storage.create<TestContract>(Ident{"set"});

    {
      auto& setContract = storage.load<TestContract>(Ident{"set"});
      setContract.s.insert(10);
      setContract.s.insert(20);
      setContract.s.insert(30);
      storage.flush();
    }
    {
      storage.switchToApply();
      auto& setContract = storage.load<TestContract>(Ident{"set"});
      // s.insert(30);
      // NOCOMMIT_LOG("Before Add 40");
      setContract.s.insert(40);
      // NOCOMMIT_LOG("After Add 40");
      // NOCOMMIT_LOG("================================");
      // s.pre_order(1);
      // NOCOMMIT_LOG("================================");
      TS_ASSERT_EQUALS(4, setContract.s.size());
      storage.flush();
    }

    {
      storage.switchToApply();
      auto& setContract = storage.load<TestContract>(Ident{"set"});
      // NOCOMMIT_LOG("================================");
      // s.pre_order(1);
      // NOCOMMIT_LOG("================================");
      setContract.s.insert(50);
      // NOCOMMIT_LOG("================================");
      // s.pre_order(1);
      // NOCOMMIT_LOG("================================");
      setContract.s.insert(25);
      storage.flush();
    }
    {
      storage.switchToApply();
      auto& setContract = storage.load<TestContract>(Ident{"set"});
      // s.pre_order(1);
      TS_ASSERT_EQUALS(6, setContract.s.size());
    }
    {
    }
  }
  void testShapeOfTree()
  {
    StorageMap storage;
    storage.switchToApply();
    storage.create<TestContract>(Ident{"set"});

    {
      storage.switchToApply();
      auto& setContract = storage.load<TestContract>(Ident{"set"});
      setContract.s.insert(11);
      setContract.s.insert(7);
      setContract.s.insert(12);
      setContract.s.insert(2);
      setContract.s.insert(8);
      setContract.s.insert(13);
      setContract.s.insert(1);
      setContract.s.insert(3);
      setContract.s.insert(4);
      storage.flush();
    }
    // {
    //   storage.switchToApply();
    //   auto& setContract = storage.load<TestContract>(Ident{"set"});
    //   // NOCOMMIT_LOG("====================================");
    //   // setContract.s.pre_order(1);
    //   // NOCOMMIT_LOG("====================================");
    //   // setContract.s.erase(12);
    // }
    // {
    //   storage.switchToApply();
    //   auto& setContract = storage.load<TestContract>(Ident{"set"});
    //   // NOCOMMIT_LOG("====================================");
    //   // setContract.s.pre_order(8);
    //   // NOCOMMIT_LOG("====================================");
    // }
  }

  void testIterator()
  {
    StorageMap storage;
    storage.switchToApply();
    storage.create<TestContract>(Ident{"set"});

    {
      storage.switchToApply();
      auto& setContract = storage.load<TestContract>(Ident{"set"});
      setContract.s.insert(11);
      setContract.s.insert(7);
      setContract.s.insert(12);
      setContract.s.insert(2);
      setContract.s.insert(8);
      setContract.s.insert(13);
      setContract.s.insert(1);
      setContract.s.insert(3);
      setContract.s.insert(4);
      storage.flush();
    }
    // {
    //   storage.switchToApply();
    //   auto& setContract = storage.load<TestContract>(Ident{"set"});
    //   auto it = setContract.s.begin();
    //   NOCOMMIT_LOG("Begin {}", *it);
    //   while (*it) {
    //     NOCOMMIT_LOG("{}", *it);
    //     ++it;
    //   }

    //   auto it2 = setContract.s.last();
    //   NOCOMMIT_LOG("LAst {}", *it2);
    //   while (*it2) {
    //     NOCOMMIT_LOG("{}", *it2);
    //     --it2;
    //   }
    // }
  }
};
