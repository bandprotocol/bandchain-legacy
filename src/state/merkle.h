#pragma once

#include <boost/optional.hpp>
#include <memory>
// #include <optional>
// #include <variant>

#include "state/hashable.h"

class MerkleTree
{
public:
  /// Add a hashable data to this data structure. If there's a hash collision,
  /// this method is a no-op and returns false. Return true otherwise.
  bool add(std::unique_ptr<Hashable> data);

  /// Find the hashable data corresponding to the given key. Return the
  /// reference on to nil if the key does not exist. The reference will be
  /// valid if the data is removed from the tree.
  Hashable& find(const Hash& key);

  /// Return the hash of this merkle tree's root.
  Hash hash() const { return root->hash(); }

  /// Return the string representation of this merkle tree.
  std::string to_string() const { return root->to_string(); }

private:
  struct MerkleNode {
    std::unique_ptr<Hashable> data;
    std::array<std::unique_ptr<MerkleNode>, 2> children;

    Hash hash() const;
    std::string to_string() const;
  };

private:
  static inline HashableNil NIL;

  std::unique_ptr<MerkleNode> root = std::make_unique<MerkleNode>();
};
