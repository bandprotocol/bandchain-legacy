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

#include <enum/enum.h>

#include "inc/essential.h"
#include "store/storage.h"
#include "util/buffer.h"
#include "util/bytes.h"

/// Shorthand macro to define Set field inside of contract.
#define SET(VAL, NAME) Set<VAL> NAME{storage, key + "/" + #NAME + "/"};

ENUM(SetCacheStatus, uint8_t, Unchanged, Changed, Erased)

/// Set is a data structure to store value in binary tree that have
/// functionality like std::set. It store value on key-value storage. Unlike
/// DataMap, this data structure can iterate over each element in order of tree
/// traversal by custom iterator.
template <typename T>
class Set
{
public:
  // Custom iterator have operator to get value(real value not a reference),
  // oprerator ++ and -- to access next and previous member.
  class Iterator
  {
  public:
    Iterator(uint64_t _currentNonce, const Set& _set)
        : currentNonce(_currentNonce)
        , set(_set)
    {
    }

    /// Operator * to get copy of value in node
    T operator*()
    {
      if (currentNonce == 0)
        return T{};
      return set.getNode(currentNonce).val;
    }

    /// Operator ++ move iterator to next element (sort by value).
    Iterator& operator++()
    {
      if (currentNonce == 0)
        return *this;
      const Node& node = set.getNode(currentNonce);
      if (node.right == 0) {
        uint64_t now = currentNonce;

        while (true) {
          const Node& nowNode = set.getNode(now);
          uint64_t parent = nowNode.parent;
          if (parent == 0) {
            currentNonce = 0;
            return *this;
          }

          const Node& parentNode = set.getNode(parent);
          if (parentNode.right == now) {
            now = parent;
          } else {
            currentNonce = parent;
            return *this;
          }
        }
      } else {
        uint64_t now = node.right;
        while (true) {
          const Node& nowNode = set.getNode(now);
          if (nowNode.left == 0) {
            currentNonce = now;
            return *this;
          } else {
            now = nowNode.left;
          }
        }
      }
    }

    /// Operator -- move iterator to previous element (sort by value).
    Iterator& operator--()
    {
      if (currentNonce == 0)
        return *this;
      const Node& node = set.getNode(currentNonce);
      if (node.left == 0) {
        uint64_t now = currentNonce;

        while (true) {
          const Node& nowNode = set.getNode(now);
          uint64_t parent = nowNode.parent;
          if (parent == 0) {
            currentNonce = 0;
            return *this;
          }

          const Node& parentNode = set.getNode(parent);
          if (parentNode.left == now) {
            now = parent;
          } else {
            currentNonce = parent;
            return *this;
          }
        }
      } else {
        uint64_t now = node.left;
        while (true) {
          const Node& nowNode = set.getNode(now);
          if (nowNode.right == 0) {
            currentNonce = now;
            return *this;
          } else {
            now = nowNode.right;
          }
        }
      }
    }

  private:
    uint64_t currentNonce;
    const Set& set;
  };

public:
  /// Create this data structure based on the given key and the storage
  /// reference. Get details of this graph using key/details as key in storage.
  Set(Storage& _storage, const std::string& _key)
      : storage(_storage)
      , key(_key)
  {

    auto result = storage.get(key + "details");
    if (result) {
      Buffer buf(gsl::as_bytes(gsl::make_span(*result)));
      buf >> nonceNode >> nonceRoot >> setSize;
    } else {
      nonceNode = 0;
      nonceRoot = 0;
      setSize = 0;
    }
  }

  /// Deconstructor response in save detail of tree and updated node on tree.
  ~Set()
  {
    if (storage.shouldFlush()) {
      Buffer buf;
      buf << nonceNode << nonceRoot << setSize;
      storage.put(key + "details", buf.to_raw_string());
      for (auto& [id, node] : cache) {
        if (node.status == SetCacheStatus::Changed) {
          DEBUG(log, "PUT {} -> {}", key + std::to_string(id), node.val);
          storage.put(key + std::to_string(id), Buffer::serialize<Node>(node));
        } else if (node.status == SetCacheStatus::Erased) {
          DEBUG(log, "DEL {}", key + std::to_string(id));
          storage.del(key + std::to_string(id));
        }
      }
    }
  }

  /// Insert new element to tree. If it has existed, return false.
  bool insert(const T& val)
  {
    uint64_t beforeInsertSize = setSize;
    nonceRoot = insertNode(nonceRoot, val);
    return beforeInsertSize != setSize;
  }

  /// Erase element on tree. If it doesn't exist, return false.
  bool erase(const T& val)
  {
    uint64_t beforeInsertSize = setSize;
    nonceRoot = deleteNode(nonceRoot, val);
    return beforeInsertSize != setSize;
    return 0;
  }

  /// Check val exist in tree. Return true if exist.
  bool contains(const T& val)
  {
    uint64_t currentNonceNode = nonceRoot;
    while (true) {
      if (currentNonceNode == 0)
        return false;

      Node& currentNode = getNode(currentNonceNode);
      if (val == currentNode.val) {
        return true;
      }

      if (val < currentNode.val) {
        currentNonceNode = currentNode.left;
      } else {
        currentNonceNode = currentNode.right;
      }
    }
  }

  /// Get the max value on tree.
  T maxValue()
  {
    if (setSize == 0)
      return T{};
    uint64_t currentNonceNode = nonceRoot;
    while (true) {
      Node& currentNode = getNode(currentNonceNode);
      if (currentNode.right == 0) {
        return currentNode.val;
      }
      currentNonceNode = currentNode.right;
    }
  }

  /// Get size of tree.
  uint64_t size() const
  {
    return setSize;
  }

  /// Find element in tree return custom iterator to this val, otherwise return
  /// id = 0
  Iterator find(const T& val)
  {
    uint64_t currentNonceNode = nonceRoot;
    while (true) {
      if (currentNonceNode == 0)
        return Iterator(0, *this);

      Node& currentNode = getNode(currentNonceNode);
      if (val == currentNode.val) {
        return Iterator(currentNonceNode, *this);
      }

      if (val < currentNode.val) {
        currentNonceNode = currentNode.left;
      } else {
        currentNonceNode = currentNode.right;
      }
    }
  }

  /// Return iterator that points to first element.
  Iterator begin()
  {
    uint64_t currentNonceNode = nonceRoot;
    while (true) {
      Node& currentNode = getNode(currentNonceNode);
      if (currentNode.left) {
        currentNonceNode = currentNode.left;
      } else {
        return Iterator(currentNonceNode, *this);
      }
    }
  }

  // Return iterator that points to last element.
  Iterator last() const
  {
    uint64_t currentNonceNode = nonceRoot;
    while (true) {
      const Node& currentNode = getNode(currentNonceNode);
      if (currentNode.right) {
        currentNonceNode = currentNode.right;
      } else {
        return Iterator(currentNonceNode, *this);
      }
    }
  }

private:
  struct Node;

  /// Return reference of Node struct.
  const Node& getNode(uint64_t nodeID) const
  {
    if (auto it = cache.find(nodeID); it != cache.end()) {
      return it->second;
    }

    auto result = storage.get(key + std::to_string(nodeID));
    if (!result)
      throw Error("Node not found.");
    return cache.emplace(nodeID, Buffer::deserialize<Node>(*result))
        .first->second;
  }

  Node& getNode(uint64_t nodeID)
  {
    return const_cast<Node&>(static_cast<const Set*>(this)->getNode(nodeID));
  }

  /// Create new node save only value other attribute will be updated by other
  /// function.
  uint64_t newNode(const T& val)
  {
    nonceNode++;
    setSize++;
    cache.emplace(nonceNode, Node{0, 0, 1, 0, val, SetCacheStatus::Changed});
    return nonceNode;
  }

  /// Get height of node
  uint64_t getHeight(uint64_t nodeNonce)
  {
    if (nodeNonce == 0)
      return 0;
    return getNode(nodeNonce).height;
  }

  /// Return nodeID of minimum node on subtree that have rootNonce as a root.
  uint64_t minValueNode(uint64_t rootNonce)
  {
    while (true) {
      Node& currentNode = getNode(rootNonce);
      if (currentNode.left == 0)
        return rootNonce;
      rootNonce = currentNode.left;
    }
  }

  /// Right rotate use in balancing process.
  uint64_t rightRotate(uint64_t y)
  {
    Node& topNode = getNode(y);
    uint64_t x = topNode.left;
    Node& leftNode = getNode(x);

    uint64_t T2 = leftNode.right;

    topNode.left = T2;
    leftNode.right = y;

    // Update parent
    uint64_t grandparent = topNode.parent;
    leftNode.parent = grandparent;
    topNode.parent = x;

    if (T2 != 0) {
      Node& tmp = getNode(T2);
      tmp.parent = y;
      tmp.status = SetCacheStatus::Changed;
    }

    topNode.height =
        std::max(getHeight(topNode.left), getHeight(topNode.right)) + 1;
    leftNode.height =
        std::max(getHeight(leftNode.left), getHeight(leftNode.right)) + 1;

    topNode.status = SetCacheStatus::Changed;
    leftNode.status = SetCacheStatus::Changed;

    return x;
  }

  /// Left rotate use in balancing process.
  uint64_t leftRotate(uint64_t x)
  {
    Node& topNode = getNode(x);
    uint64_t y = topNode.right;
    Node& rightNode = getNode(y);

    uint64_t T2 = rightNode.left;

    topNode.right = T2;
    rightNode.left = x;

    // Update parent
    uint64_t grandparent = topNode.parent;
    rightNode.parent = grandparent;
    topNode.parent = y;

    if (T2 != 0) {
      Node& tmp = getNode(T2);
      tmp.parent = x;
      tmp.status = SetCacheStatus::Changed;
    }

    topNode.height =
        std::max(getHeight(topNode.left), getHeight(topNode.right)) + 1;
    rightNode.height =
        std::max(getHeight(rightNode.left), getHeight(rightNode.right)) + 1;

    topNode.status = SetCacheStatus::Changed;
    rightNode.status = SetCacheStatus::Changed;

    return y;
  }

  /// Find difference of height between left and right subtree. Return true of
  /// leftHeight >= rightHeight and return difference height in second member.
  std::pair<bool, uint64_t> getBalance(uint64_t topNodeNonce)
  {
    if (topNodeNonce == 0)
      return {true, 0};
    Node& node = getNode(topNodeNonce);
    uint64_t leftHeight = getHeight(node.left);
    uint64_t rightHeight = getHeight(node.right);

    if (leftHeight >= rightHeight)
      return {true, leftHeight - rightHeight};
    else
      return {false, rightHeight - leftHeight};
  }

  /// Update parent to given child node.
  void updateParent(uint64_t parent, uint64_t child)
  {
    if (child == 0)
      return;
    Node& childNode = getNode(child);
    childNode.parent = parent;
  }

  // Insert new value and return new rootNonce
  uint64_t insertNode(uint64_t currentNonce, const T& val)
  {
    if (currentNonce == 0) {
      return newNode(val);
    }

    Node& currentNode = getNode(currentNonce);
    if (val < currentNode.val) {
      currentNode.left = insertNode(currentNode.left, val);
      updateParent(currentNonce, currentNode.left);
    } else if (val > currentNode.val) {
      currentNode.right = insertNode(currentNode.right, val);
      updateParent(currentNonce, currentNode.right);
    } else {
      return currentNonce;
    }

    currentNode.height =
        1 + std::max(getHeight(currentNode.left), getHeight(currentNode.right));
    currentNode.status = SetCacheStatus::Changed;

    auto [heavyLeft, heightDiff] = getBalance(currentNonce);

    // Left
    if (heavyLeft && heightDiff > 1) {
      Node& leftNode = getNode(currentNode.left);
      // Left Left Case
      if (val < leftNode.val) {
        return rightRotate(currentNonce);
      }
      // Left Right Case
      else if (val > leftNode.val) {
        currentNode.left = leftRotate(currentNode.left);
        return rightRotate(currentNonce);
      }
    }

    // Right
    if (!heavyLeft && heightDiff > 1) {
      Node& rightNode = getNode(currentNode.right);
      // Right Right Case
      if (val > rightNode.val) {
        return leftRotate(currentNonce);
      } else if (val < rightNode.val) {
        currentNode.right = rightRotate(currentNode.right);
        return leftRotate(currentNonce);
      }
    }

    return currentNonce;
  }

  // Delete node return new rootNonce

  uint64_t deleteNode(uint64_t currentNonce, const T& val)
  {
    if (currentNonce == 0)
      return 0;

    Node& currentNode = getNode(currentNonce);
    if (val < currentNode.val) {
      currentNode.left = deleteNode(currentNode.left, val);
      updateParent(currentNonce, currentNode.left);
    } else if (val > currentNode.val) {
      currentNode.right = deleteNode(currentNode.right, val);
      updateParent(currentNonce, currentNode.right);
    } else {
      if (currentNode.left == 0 || currentNode.right == 0) {
        uint64_t child =
            currentNode.left ? currentNode.left : currentNode.right;
        // No child case
        uint64_t deletedNodeNonce;
        if (child == 0) {
          deletedNodeNonce = currentNonce;
        } else {
          Node& childNode = getNode(child);
          uint64_t oldParent = currentNode.parent;
          currentNode = childNode;
          currentNode.parent = oldParent;
          currentNode.status = SetCacheStatus::Changed;
          deletedNodeNonce = child;
        }

        // Delete node
        setSize--;
        Node& deleteNode = getNode(deletedNodeNonce);
        deleteNode.status = SetCacheStatus::Erased;
      } else {
        Node& minNode = getNode(minValueNode(currentNode.right));

        currentNode.val = minNode.val;
        currentNode.right = deleteNode(currentNode.right, currentNode.val);
      }
    }

    if (currentNode.status == SetCacheStatus::Erased)
      return 0;

    currentNode.height =
        1 + std::max(getHeight(currentNode.left), getHeight(currentNode.right));
    currentNode.status = SetCacheStatus::Changed;

    // Make tree balance
    auto [heavyLeft, heightDiff] = getBalance(currentNonce);

    // Left Left Case
    if (heavyLeft && heightDiff > 1 && getBalance(currentNode.left).first)
      return rightRotate(currentNonce);

    // Left Right Case
    if (heavyLeft && heightDiff > 1 && !getBalance(currentNode.left).first) {
      currentNode.left = leftRotate(currentNode.left);
      return rightRotate(currentNonce);
    }

    // Right Right Case
    if (!heavyLeft && heightDiff > 1 && !getBalance(currentNode.right).first)
      return leftRotate(currentNonce);

    // Right Left Case
    if (!heavyLeft && heightDiff > 1 && getBalance(currentNode.right).first) {
      currentNode.right = rightRotate(currentNode.right);
      return leftRotate(currentNonce);
    }

    return currentNonce;
  }

private:
  /// Struct Node represents node data in AVL-tree.
  struct Node {
    uint64_t left;
    uint64_t right;
    uint64_t height;
    uint64_t parent;
    T val;
    SetCacheStatus status = SetCacheStatus::Unchanged;
  };

  /// operator << , >> for store each node to storage.
  friend Buffer& operator<<(Buffer& buf, const Node& node)
  {
    return buf << node.left << node.right << node.height << node.parent
               << node.val;
  }

  friend Buffer& operator>>(Buffer& buf, Node& node)
  {
    return buf >> node.left >> node.right >> node.height >> node.parent >>
           node.val;
  }

  /// Reference to the storage layer.
  Storage& storage;

  /// The key to which this set use to access data.
  const std::string key;

  /// Detail about avl-tree
  uint64_t nonceNode;
  uint64_t nonceRoot;
  uint64_t setSize;

  /// The cache value containing changed/erased node in tree that need to
  /// save.
  mutable std::unordered_map<uint64_t, Node> cache;

  /// Static logger for this class.
  static inline auto log = logger::get("set");
};
