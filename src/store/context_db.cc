#include "context_db.h"

#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/transaction_db.h"
#include "util/cli.h"

namespace
{
rocksdb::Slice get_slice_from_hash(const Hash& key)
{
  auto key_span = key.as_span();
  return rocksdb::Slice((const char*)key_span.data(), key_span.size_bytes());
}
} // namespace

CmdArg<std::string> db_path("db-path", "rocksdb path");

ContextDB::ContextDB()
{
  rocksdb::Options options;
  rocksdb::TransactionDBOptions txn_db_options;
  options.create_if_missing = true;
  rocksdb::TransactionDB* db;

  rocksdb::Status s =
      rocksdb::TransactionDB::Open(options, txn_db_options, +db_path, &db);
  assert(s.ok());

  txn_db.reset(db);
}

ContextDB::~ContextDB() {}

uint64_t ContextDB::height() const
{
  // TODO
  return 0;
}

Hash ContextDB::root() const
{
  // TODO
  return {};
}

void ContextDB::commit()
{
  // TODO
}

std::pair<std::string, bool> ContextDB::try_get(const Hash& key) const
{
  std::string value;
  auto slice_key = get_slice_from_hash(key);
  rocksdb::Status s = txn_db->Get(rocksdb::ReadOptions(), slice_key, &value);
  if (s.IsNotFound()) {
    return {{}, false};
  } else {
    return {value, true};
  }
}

std::string ContextDB::get(const Hash& key) const
{
  std::string value;
  auto slice_key = get_slice_from_hash(key);
  rocksdb::Status s = txn_db->Get(rocksdb::ReadOptions(), slice_key, &value);
  if (s.IsNotFound()) {
    throw Error("Context::get: key {} does not exist", key);
  } else {
    return value;
  }
}

bool ContextDB::check(const Hash& key) const
{
  std::string value;
  auto slice_key = get_slice_from_hash(key);
  rocksdb::Status s = txn_db->Get(rocksdb::ReadOptions(), slice_key, &value);
  return !s.IsNotFound();
}

void ContextDB::add(const Hash& key, const std::string value)
{
  std::string v;
  auto slice_key = get_slice_from_hash(key);
  rocksdb::Status s = txn_db->Get(rocksdb::ReadOptions(), slice_key, &v);
  if (s.IsNotFound()) {
    s = txn_db->Put(rocksdb::WriteOptions(), slice_key, value);
  } else {
    throw Error("Context::add: key {} already exists", key);
  }
}

void ContextDB::set(const Hash& key, const std::string value)
{
  rocksdb::Status s =
      txn_db->Put(rocksdb::WriteOptions(), get_slice_from_hash(key), value);
}
