// TODO

// #pragma once

// #include <memory>

// #include "state/objectx.h"

// class MerkleTree
// {
// public:
//   /// Load this merkle tree of the root hash. Throw on failure.
//   static MerkleTree load(const Hash& root);

//   /// Return a new tree with the given object added. Throw if the given
//   object's
//   /// hash collides with an existing object.
//   MerkleTree add(std::unique_ptr<Object> data) const;

//   /// Return a new tree with the given object replaced an existing object
//   /// with the same hash. Throw if there is no existing object.
//   MerkleTree replace(std::unique_ptr<Object> data) const;

//   /// Save this merkle tree to the local database. After this operation
//   /// succeeds, the tree should be loadable by root hash.
//   void save() const;

//   /// Check if the given key exists in this structure.
//   bool contains(const Hash& key) const { return find_object(key) != nullptr;
//   }

//   /// Find the object as the given type. Throw on failure.
//   template <typename ObjectT,
//             typename = std::enable_if_t<std::is_base_of_v<Object, ObjectT>>>
//   const ObjectT& find(const Hash& key) const
//   {
//     const Object* base_object = find_object(key);
//     if (base_object == nullptr) {
//       throw Error("Unable to find object at {}", key);
//     }

//     const ObjectT* object = dynamic_cast<const ObjectT*>(base_object);
//     if (object == nullptr) {
//       throw Error("Object at {} is of unexpected type", key);
//     }
//     return *object;
//   }

//   /// Return the hash of this merkle tree's root.
//   Hash hash() const { return root->hash(); }

//   /// Return the string representation of this merkle tree.
//   std::string to_string() const { return root->to_string(); }

// private:
//   /// Find the object corresponding to the given key. Return nullptr on
//   failure. const Object* find_object(const Hash& key) const;

//   struct MerkleNode {
//     std::unique_ptr<Object> data;
//     std::array<std::unique_ptr<MerkleNode>, 2> children;

//     Hash hash() const;
//     std::string to_string() const;
//   };

// private:
//   std::shared_ptr<MerkleNode> root = std::make_shared<MerkleNode>();
// };
