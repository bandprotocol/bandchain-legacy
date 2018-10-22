#include "storage_rocksdb.h"

#include "util/cli.h"

CmdArg<std::string> db_path("db-path", "rocksdb path");

namespace
{
rocksdb::Slice get_slice_from_hash(const Hash& key)
{
  auto key_span = key.as_span();
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
}

void StorageDB::put(const Hash& key, const std::string& val)
{
  rocksdb::Status s = txn->Put(get_slice_from_hash(key), val);
}

nonstd::optional<std::string> StorageDB::get(const Hash& key) const
{
  std::string value;
  auto slice_key = get_slice_from_hash(key);
  rocksdb::Status s = txn->Get(rocksdb::ReadOptions(), slice_key, &value);
  if (s.IsNotFound()) {
    return nonstd::nullopt;
  } else {
    return value;
  }
}

void StorageDB::del(const Hash& key)
{
  rocksdb::Status s = txn->Delete(get_slice_from_hash(key));
}

void StorageDB::start_block()
{
  rocksdb::OptimisticTransactionOptions txn_options;
  txn.reset(txn_db->BeginTransaction(rocksdb::WriteOptions(), txn_options));
}

void StorageDB::end_block()
{
  rocksdb::Status s = txn->Commit();
  if (!s.ok()) {
    throw Failure("Cannot save data to DB.");
  }
  txn.reset();
}

void StorageDB::save_protected_key(const std::string& key,
                                   const std::string& val)
{
  rocksdb::Status s = txn_db->Put(rocksdb::WriteOptions(), key, val);
}

nonstd::optional<std::string>
StorageDB::get_protected_key(const std::string& key)
{
  std::string value;
  rocksdb::Status s = txn_db->Get(rocksdb::ReadOptions(), key, &value);
  if (s.IsNotFound()) {
    return nonstd::nullopt;
  } else {
    return value;
  }
  return nonstd::nullopt;
}
