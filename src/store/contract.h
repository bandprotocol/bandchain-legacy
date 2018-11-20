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

#pragma once

#include "inc/essential.h"

/// Forward declaration here. See "store/storage.h" for details.
class Storage;

/// Contract class is a thin wrapper on top of the storage to provide the
/// external code an abstract and systemetic way to access and modify data.
class Contract
{
public:
  /// Create the contract. Again this is not actually "creating" the contract,
  /// but rather creating a wrapper that allows external code to inspect the
  /// contract.
  Contract(Storage& storage, const std::string& key);

  /// Virtual destructor to ensure this class has vtable.
  virtual ~Contract() {}

  /// Cast this contract instance to the parameterized type. Since contract
  /// class contains a virtual function, dynamic_cast can be perform here.
  template <typename T>
  T* as()
  {
    T* result = dynamic_cast<T*>(this);
    if (result == nullptr)
      throw Error("Contract::as: invalid contract cast");

    return result;
  }

protected:
  /// Reference to the storage layer.
  Storage& storage;

  /// The key of this contract.
  const std::string key;
};
