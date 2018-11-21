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

#include "storage_map.h"

nonstd::optional<std::string> StorageMap::get(const std::string& key) const
{
  if (auto it = currentCache->find(key); it != currentCache->end())
    return it->second;

  return nonstd::nullopt;
}

void StorageMap::put(const std::string& key, const std::string& val)
{
  currentCache->operator[](key) = val;
}

void StorageMap::del(const std::string& key)
{
  currentCache->erase(key);
}

void StorageMap::commit()
{
  checkCache = applyCache;
  currentCache = nullptr;
}

void StorageMap::switchToCheck()
{
  currentCache = &checkCache;
}

void StorageMap::switchToApply()
{
  currentCache = &applyCache;
}
