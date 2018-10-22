#pragma once

#include <nonstd/optional.hpp>
#include <unordered_map>

#include "rocksdb/db.h"
#include "rocksdb/utilities/optimistic_transaction_db.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/transaction_db.h"
#include "store/storage.h"

class StorageDB : public Storage
{
public:
  StorageDB();

  void put(const Hash& key, const std::string& val) final;
  nonstd::optional<std::string> get(const Hash& key) const final;
  void del(const Hash& key) final;

  void start_block() final;
  void end_block() final;
  void save_protected_key(const std::string& key, const std::string& val) final;
  nonstd::optional<std::string> get_protected_key(const std::string& key) final;

public:
  std::unique_ptr<rocksdb::OptimisticTransactionDB> txn_db;
  std::unique_ptr<rocksdb::Transaction> txn;
};
