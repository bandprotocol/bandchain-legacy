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
    Iterator(uint64_t _current_nonce, const Set& _set)
        : current_nonce(_current_nonce)
        , set(_set)
    {
    }

    /// Operator * to get copy of value in node
    T operator*()
    {
      if (current_nonce == 0)
        return T{};
      return set.get_node(current_nonce).val;
    }

    /// Operator ++ move iterator to next element (sort by value).
    Iterator& operator++()
    {
      if (current_nonce == 0)
        return *this;
      const Node& node = set.get_node(current_nonce);
      if (node.right == 0) {
        uint64_t now = current_nonce;

        while (true) {
          const Node& now_node = set.get_node(now);
          uint64_t parent = now_node.parent;
          if (parent == 0) {
            current_nonce = 0;
            return *this;
          }

          const Node& parent_node = set.get_node(parent);
          if (parent_node.right == now) {
            now = parent;
          } else {
            current_nonce = parent;
            return *this;
          }
        }
      } else {
        uint64_t now = node.right;
        while (true) {
          const Node& now_node = set.get_node(now);
          if (now_node.left == 0) {
            current_nonce = now;
            return *this;
          } else {
            now = now_node.left;
          }
        }
      }
    }

    /// Operator -- move iterator to previous element (sort by value).
    Iterator& operator--()
    {
      if (current_nonce == 0)
        return *this;
      const Node& node = set.get_node(current_nonce);
      if (node.left == 0) {
        uint64_t now = current_nonce;

        while (true) {
          const Node& now_node = set.get_node(now);
          uint64_t parent = now_node.parent;
          if (parent == 0) {
            current_nonce = 0;
            return *this;
          }

          const Node& parent_node = set.get_node(parent);
          if (parent_node.left == now) {
            now = parent;
          } else {
            current_nonce = parent;
            return *this;
          }
        }
      } else {
        uint64_t now = node.left;
        while (true) {
          const Node& now_node = set.get_node(now);
          if (now_node.right == 0) {
            current_nonce = now;
            return *this;
          } else {
            now = now_node.right;
          }
        }
      }
    }

  private:
    uint64_t current_nonce;
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
      buf >> nonce_node >> nonce_root >> m_size;
    } else {
      nonce_node = 0;
      nonce_root = 0;
      m_size = 0;
    }
  }

  /// Deconstructor response in save detail of tree and updated node on tree.
  ~Set()
  {
    if (storage.shouldFlush()) {
      Buffer buf;
      buf << nonce_node << nonce_root << m_size;
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
    uint64_t before_insert_size = m_size;
    nonce_root = insert_node(nonce_root, val);
    return before_insert_size != m_size;
  }

  /// Erase element on tree. If it doesn't exist, return false.
  bool erase(const T& val)
  {
    uint64_t before_insert_size = m_size;
    nonce_root = delete_node(nonce_root, val);
    return before_insert_size != m_size;
    return 0;
  }

  /// Check val exist in tree. Return true if exist.
  bool contains(const T& val)
  {
    uint64_t current_nonce_node = nonce_root;
    while (true) {
      if (current_nonce_node == 0)
        return false;

      Node& current_node = get_node(current_nonce_node);
      if (val == current_node.val) {
        return true;
      }

      if (val < current_node.val) {
        current_nonce_node = current_node.left;
      } else {
        current_nonce_node = current_node.right;
      }
    }
  }

  /// Get the max value on tree.
  T get_max()
  {
    if (m_size == 0)
      return T{};
    uint64_t current_nonce_node = nonce_root;
    while (true) {
      Node& current_node = get_node(current_nonce_node);
      if (current_node.right == 0) {
        return current_node.val;
      }
      current_nonce_node = current_node.right;
    }
  }

  /// Get size of tree.
  uint64_t size() const
  {
    return m_size;
  }

  /// Find element in tree return custom iterator to this val, otherwise return
  /// id = 0
  Iterator find(const T& val)
  {
    uint64_t current_nonce_node = nonce_root;
    while (true) {
      if (current_nonce_node == 0)
        return Iterator(0, *this);

      Node& current_node = get_node(current_nonce_node);
      if (val == current_node.val) {
        return Iterator(current_nonce_node, *this);
      }

      if (val < current_node.val) {
        current_nonce_node = current_node.left;
      } else {
        current_nonce_node = current_node.right;
      }
    }
  }

  /// Return iterator that points to first element.
  Iterator begin()
  {
    uint64_t current_nonce_node = nonce_root;
    while (true) {
      Node& current_node = get_node(current_nonce_node);
      if (current_node.left) {
        current_nonce_node = current_node.left;
      } else {
        return Iterator(current_nonce_node, *this);
      }
    }
  }

  // Return iterator that points to last element.
  Iterator last() const
  {
    uint64_t current_nonce_node = nonce_root;
    while (true) {
      const Node& current_node = get_node(current_nonce_node);
      if (current_node.right) {
        current_nonce_node = current_node.right;
      } else {
        return Iterator(current_nonce_node, *this);
      }
    }
  }

private:
  struct Node;

  /// Return reference of Node struct.
  const Node& get_node(uint64_t node_id) const
  {
    if (auto it = cache.find(node_id); it != cache.end()) {
      return it->second;
    }

    auto result = storage.get(key + std::to_string(node_id));
    if (!result)
      throw Error("Node not found.");
    return cache.emplace(node_id, Buffer::deserialize<Node>(*result))
        .first->second;
  }

  Node& get_node(uint64_t node_id)
  {
    return const_cast<Node&>(static_cast<const Set*>(this)->get_node(node_id));
  }

  /// Create new node save only value other attribute will be updated by other
  /// function.
  uint64_t new_node(const T& val)
  {
    nonce_node++;
    m_size++;
    cache.emplace(nonce_node, Node{0, 0, 1, 0, val, SetCacheStatus::Changed});
    return nonce_node;
  }

  /// Get height of node
  uint64_t get_height(uint64_t node_nonce)
  {
    if (node_nonce == 0)
      return 0;
    return get_node(node_nonce).height;
  }

  /// Return node_id of minimum node on subtree that have root_nonce as a root.
  uint64_t min_value_node(uint64_t root_nonce)
  {
    while (true) {
      Node& current_node = get_node(root_nonce);
      if (current_node.left == 0)
        return root_nonce;
      root_nonce = current_node.left;
    }
  }

  /// Right rotate use in balancing process.
  uint64_t right_rotate(uint64_t y)
  {
    Node& top_node = get_node(y);
    uint64_t x = top_node.left;
    Node& left_node = get_node(x);

    uint64_t T2 = left_node.right;

    top_node.left = T2;
    left_node.right = y;

    // Update parent
    uint64_t grandparent = top_node.parent;
    left_node.parent = grandparent;
    top_node.parent = x;

    if (T2 != 0) {
      Node& tmp = get_node(T2);
      tmp.parent = y;
      tmp.status = SetCacheStatus::Changed;
    }

    top_node.height =
        std::max(get_height(top_node.left), get_height(top_node.right)) + 1;
    left_node.height =
        std::max(get_height(left_node.left), get_height(left_node.right)) + 1;

    top_node.status = SetCacheStatus::Changed;
    left_node.status = SetCacheStatus::Changed;

    return x;
  }

  /// Left rotate use in balancing process.
  uint64_t left_rotate(uint64_t x)
  {
    Node& top_node = get_node(x);
    uint64_t y = top_node.right;
    Node& right_node = get_node(y);

    uint64_t T2 = right_node.left;

    top_node.right = T2;
    right_node.left = x;

    // Update parent
    uint64_t grandparent = top_node.parent;
    right_node.parent = grandparent;
    top_node.parent = y;

    if (T2 != 0) {
      Node& tmp = get_node(T2);
      tmp.parent = x;
      tmp.status = SetCacheStatus::Changed;
    }

    top_node.height =
        std::max(get_height(top_node.left), get_height(top_node.right)) + 1;
    right_node.height =
        std::max(get_height(right_node.left), get_height(right_node.right)) + 1;

    top_node.status = SetCacheStatus::Changed;
    right_node.status = SetCacheStatus::Changed;

    return y;
  }

  /// Find difference of height between left and right subtree. Return true of
  /// left_height >=right_height and return difference height in second member.
  std::pair<bool, uint64_t> get_balance(uint64_t top_node_nonce)
  {
    if (top_node_nonce == 0)
      return {true, 0};
    Node& node = get_node(top_node_nonce);
    uint64_t left_height = get_height(node.left);
    uint64_t right_height = get_height(node.right);

    if (left_height >= right_height)
      return {true, left_height - right_height};
    else
      return {false, right_height - left_height};
  }

  /// Update parent to given child node.
  void update_parent(uint64_t parent, uint64_t child)
  {
    if (child == 0)
      return;
    Node& child_node = get_node(child);
    child_node.parent = parent;
  }

  // Insert new value and return new root_nonce
  uint64_t insert_node(uint64_t current_nonce, const T& val)
  {
    if (current_nonce == 0) {
      return new_node(val);
    }

    Node& current_node = get_node(current_nonce);
    if (val < current_node.val) {
      current_node.left = insert_node(current_node.left, val);
      update_parent(current_nonce, current_node.left);
    } else if (val > current_node.val) {
      current_node.right = insert_node(current_node.right, val);
      update_parent(current_nonce, current_node.right);
    } else {
      return current_nonce;
    }

    current_node.height = 1 + std::max(get_height(current_node.left),
                                       get_height(current_node.right));
    current_node.status = SetCacheStatus::Changed;

    auto [heavy_left, height_diff] = get_balance(current_nonce);

    // Left
    if (heavy_left && height_diff > 1) {
      Node& left_node = get_node(current_node.left);
      // Left Left Case
      if (val < left_node.val) {
        return right_rotate(current_nonce);
      }
      // Left Right Case
      else if (val > left_node.val) {
        current_node.left = left_rotate(current_node.left);
        return right_rotate(current_nonce);
      }
    }

    // Right
    if (!heavy_left && height_diff > 1) {
      Node& right_node = get_node(current_node.right);
      // Right Right Case
      if (val > right_node.val) {
        return left_rotate(current_nonce);
      } else if (val < right_node.val) {
        current_node.right = right_rotate(current_node.right);
        return left_rotate(current_nonce);
      }
    }

    return current_nonce;
  }

  // Delete node return new root_nonce

  uint64_t delete_node(uint64_t current_nonce, const T& val)
  {
    if (current_nonce == 0)
      return 0;

    Node& current_node = get_node(current_nonce);
    if (val < current_node.val) {
      current_node.left = delete_node(current_node.left, val);
      update_parent(current_nonce, current_node.left);
    } else if (val > current_node.val) {
      current_node.right = delete_node(current_node.right, val);
      update_parent(current_nonce, current_node.right);
    } else {
      if (current_node.left == 0 || current_node.right == 0) {
        uint64_t child =
            current_node.left ? current_node.left : current_node.right;
        // No child case
        uint64_t deleted_node_nonce;
        if (child == 0) {
          deleted_node_nonce = current_nonce;
        } else {
          Node& child_node = get_node(child);
          uint64_t old_parent = current_node.parent;
          current_node = child_node;
          current_node.parent = old_parent;
          current_node.status = SetCacheStatus::Changed;
          deleted_node_nonce = child;
        }

        // Delete node
        m_size--;
        Node& delete_node = get_node(deleted_node_nonce);
        delete_node.status = SetCacheStatus::Erased;
      } else {
        Node& min_node = get_node(min_value_node(current_node.right));

        current_node.val = min_node.val;
        current_node.right = delete_node(current_node.right, current_node.val);
      }
    }

    if (current_node.status == SetCacheStatus::Erased)
      return 0;

    current_node.height = 1 + std::max(get_height(current_node.left),
                                       get_height(current_node.right));
    current_node.status = SetCacheStatus::Changed;

    // Make tree balance
    auto [heavy_left, height_diff] = get_balance(current_nonce);

    // Left Left Case
    if (heavy_left && height_diff > 1 && get_balance(current_node.left).first)
      return right_rotate(current_nonce);

    // Left Right Case
    if (heavy_left && height_diff > 1 &&
        !get_balance(current_node.left).first) {
      current_node.left = left_rotate(current_node.left);
      return right_rotate(current_nonce);
    }

    // Right Right Case
    if (!heavy_left && height_diff > 1 &&
        !get_balance(current_node.right).first)
      return left_rotate(current_nonce);

    // Right Left Case
    if (!heavy_left && height_diff > 1 &&
        get_balance(current_node.right).first) {
      current_node.right = right_rotate(current_node.right);
      return left_rotate(current_nonce);
    }

    return current_nonce;
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
  uint64_t nonce_node;
  uint64_t nonce_root;
  uint64_t m_size;

  /// The cache value containing changed/erased node in tree that need to
  /// save.
  mutable std::unordered_map<uint64_t, Node> cache;

  /// Static logger for this class.
  static inline auto log = logger::get("set");
};
