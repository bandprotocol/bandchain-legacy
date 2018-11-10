#include <iostream>

#include "inc/essential.h"
#include "rocksdb/db.h"
#include "rocksdb/utilities/optimistic_transaction_db.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/transaction_db.h"
#include "util/buffer.h"
#include "util/cli.h"

CmdArg<std::string> db_path("db-path", "rocksdb path");
CmdArg<std::string> key("key", "key of value");

int main(int argc, char* argv[])
{
  Cmd cmd(argc, argv);

  rocksdb::Options options;
  options.create_if_missing = false;
  rocksdb::OptimisticTransactionDB* txn_db;
  rocksdb::Status s =
      rocksdb::OptimisticTransactionDB::Open(options, +db_path, &txn_db);
  if (!s.ok())
    throw Error("Cannot open database");
  std::string value;
  s = txn_db->Get(rocksdb::ReadOptions(), +key, &value);
  std::cout << Buffer::deserialize<uint256_t>(value) << std::endl;
}
