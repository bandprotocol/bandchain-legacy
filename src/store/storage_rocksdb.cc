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

#include "storage_rocksdb.h"

#include "util/cli.h"

CmdArg<std::string> db_path("db-path", "rocksdb path");

namespace
{
rocksdb::Slice get_slice_from_hash(const Hash& key)
{
  const_span key_span = key.as_const_span();
  return rocksdb::Slice((const char*)key_span.data(), key_span.size_bytes());
}
} // namespace

StorageDB::StorageDB()
{
  rocksdb::Options options;
  options.create_if_missing = true;
  rocksdb::OptimisticTransactionDB* db;
  rocksdb::Status s =
      rocksdb::OptimisticTransactionDB::Open(options, +db_path, &db);
  assert(s.ok());
  txn_db.reset(db);
  rocksdb::OptimisticTransactionOptions txn_options;
  tx_transaction.reset(
      txn_db->BeginTransaction(rocksdb::WriteOptions(), txn_options));
  check_transaction.reset(
      txn_db->BeginTransaction(rocksdb::WriteOptions(), txn_options));
  query_transaction.reset(
      txn_db->BeginTransaction(rocksdb::WriteOptions(), txn_options));
  current_transaction = tx_transaction.get();
}

void StorageDB::put(const Hash& key, const std::string& val)
{
  rocksdb::Status s = current_transaction->Put(get_slice_from_hash(key), val);
}

nonstd::optional<std::string> StorageDB::get(const Hash& key) const
{
  std::string value;
  auto slice_key = get_slice_from_hash(key);
  rocksdb::Status s =
      current_transaction->Get(rocksdb::ReadOptions(), slice_key, &value);
  if (s.IsNotFound()) {
    return nonstd::nullopt;
  } else {
    return value;
  }
}

void StorageDB::del(const Hash& key)
{
  rocksdb::Status s = current_transaction->Delete(get_slice_from_hash(key));
}

void StorageDB::commit_block()
{
  rocksdb::Status s = tx_transaction->Commit();
  if (!s.ok()) {
    throw Failure("Cannot save data to DB.");
  }

  rocksdb::OptimisticTransactionOptions txn_options;
  tx_transaction.reset(
      txn_db->BeginTransaction(rocksdb::WriteOptions(), txn_options));
  check_transaction.reset(
      txn_db->BeginTransaction(rocksdb::WriteOptions(), txn_options));
  query_transaction.reset(
      txn_db->BeginTransaction(rocksdb::WriteOptions(), txn_options));
}

void StorageDB::switch_to_tx()
{
  current_transaction = tx_transaction.get();
}

void StorageDB::switch_to_check()
{
  current_transaction = check_transaction.get();
}

void StorageDB::switch_to_query()
{
  current_transaction = query_transaction.get();
}

void StorageDB::save_protected_key(const std::string& key,
                                   const std::string& val)
{
  tx_transaction->Put(key, val);
}

nonstd::optional<std::string>
StorageDB::get_protected_key(const std::string& key)
{
  std::string value;
  rocksdb::Status s = tx_transaction->Get(rocksdb::ReadOptions(), key, &value);
  if (s.IsNotFound()) {
    return nonstd::nullopt;
  } else {
    return value;
  }
  return nonstd::nullopt;
}
