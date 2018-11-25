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
#include "store/storage_map.h"
#include "store/vector.h"
#include "util/buffer.h"
#include "util/string.h"

class TestVectorContract final : public Contract
{
public:
  using Contract::Contract;

  static constexpr char KeyPrefix[] = "test/";

  void init() {}
  VECTOR(uint16_t, v)
};

class VectorTest : public CxxTest::TestSuite
{
public:
  void testPushBack()
  {
    StorageMap storage;
    storage.switchToApply();
    {
      storage.create<TestVectorContract>(Ident{"vector"});
    }
    {
      storage.switchToApply();
      auto& testContract = storage.load<TestVectorContract>(Ident{"vector"});
      testContract.v.pushBack(5);
      testContract.v.pushBack(72);
      storage.flush();
    }
    {
      storage.switchToApply();
      auto& testContract = storage.load<TestVectorContract>(Ident{"vector"});

      TS_ASSERT_EQUALS(2, testContract.v.size());
      TS_ASSERT_EQUALS(5, testContract.v[0]);
      TS_ASSERT_EQUALS(72, testContract.v[1]);
    }
  }

  void testLowerBound()
  {
    StorageMap storage;
    storage.switchToApply();
    {
      storage.create<TestVectorContract>(Ident{"vector"});
    }
    {
      storage.switchToApply();
      auto& testContract = storage.load<TestVectorContract>(Ident{"vector"});
      testContract.v.pushBack(5);
      testContract.v.pushBack(9);
      testContract.v.pushBack(9);
      testContract.v.pushBack(12);
      testContract.v.pushBack(13);
      storage.flush();
    }
    {
      storage.switchToApply();
      auto& testContract = storage.load<TestVectorContract>(Ident{"vector"});

      TS_ASSERT_EQUALS(0, testContract.v.lowerBoundIndex(0));
      TS_ASSERT_EQUALS(0, testContract.v.lowerBoundIndex(4));
      TS_ASSERT_EQUALS(0, testContract.v.lowerBoundIndex(5));
      TS_ASSERT_EQUALS(1, testContract.v.lowerBoundIndex(6));
      TS_ASSERT_EQUALS(1, testContract.v.lowerBoundIndex(9));
      TS_ASSERT_EQUALS(3, testContract.v.lowerBoundIndex(10));
      TS_ASSERT_EQUALS(3, testContract.v.lowerBoundIndex(12));
      TS_ASSERT_EQUALS(4, testContract.v.lowerBoundIndex(13));
      TS_ASSERT_EQUALS(5, testContract.v.lowerBoundIndex(14));
      TS_ASSERT_EQUALS(5, testContract.v.lowerBoundIndex(100));
    }
  }
};
