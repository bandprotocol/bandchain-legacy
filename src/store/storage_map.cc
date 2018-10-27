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

#include "crypto/sha256.h"

void StorageMap::put(const Hash& key, const std::string& val)
{
  data[key] = val;
}

nonstd::optional<std::string> StorageMap::get(const Hash& key) const
{
  if (auto it = data.find(key); it != data.end())
    return it->second;
  else
    return nonstd::nullopt;
}

void StorageMap::del(const Hash& key)
{
  data.erase(key);
}

void StorageMap::save_protected_key(const std::string& key,
                                    const std::string& val)
{
  data[sha256(gsl::make_span(key))] = val;
}

nonstd::optional<std::string>
StorageMap::get_protected_key(const std::string& key)
{
  return get(sha256(gsl::make_span(key)));
}
