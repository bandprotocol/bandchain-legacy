#include <cassert>
#include <iostream>

#include "rocksdb/db.h"

using namespace std;

int main()
{
  rocksdb::DB* db;
  rocksdb::Options options;
  options.create_if_missing = true;
  // options.error_if_exists = true;
  rocksdb::Status status = rocksdb::DB::Open(options, "/tmp/testdb", &db);
  assert(status.ok());

  std::string value;
  rocksdb::Status s = db->Put(rocksdb::WriteOptions(), "key2", "value");
  s = db->Get(rocksdb::ReadOptions(), "key2", &value);
  cout << value;
  s = db->Put(rocksdb::WriteOptions(), "P Swiss", "So Smart");
  s = db->Get(rocksdb::ReadOptions(), "P Swiss", &value);
  cout << value;
  // if (s.ok()) s = db->Put(rocksdb::WriteOptions(), key2, value);
  // if (s.ok()) s = db->Delete(rocksdb::WriteOptions(), key1);

  delete db;
}

// int main()
// {

// }
