// #pragma once

// #include "inc/essential.h"
// #include "store/context.h"

// namespace rocksdb
// {
// class TransactionDB;
// }

// class ContextDB : public Context
// {
// public:
//   ContextDB();
//   ~ContextDB();

//   /// Get the current blockchain height.
//   uint64_t height() const final;

//   /// Get the current merkle root of the blockchain.
//   Hash root() const final;

//   /// Commit all changes to persistent data store, and increment the block
//   /// height by one.
//   void commit() final;

//   /// Return the data stored at the given key, and boolean indicating the
//   /// success.
//   std::pair<std::string, bool> try_get(const Hash& key) const final;

//   /// Return the data stored at the given key. Throw of the key does not
//   exist. std::string get(const Hash& key) const final;

//   /// Return true iff the key exists in the database.
//   bool check(const Hash& key) const final;

//   /// Add the data stored at the given key. Throw if the key already exists.
//   void add(const Hash& key, const std::string value) final;

//   /// Similar to add, but don't throw.
//   void set(const Hash& key, const std::string value) final;

// private:
//   std::unique_ptr<rocksdb::TransactionDB> txn_db;
// };
