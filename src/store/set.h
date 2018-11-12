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

#include "inc/essential.h"
#include "store/global.h"
#include "util/buffer.h"
#include "util/bytes.h"

BETTER_ENUM(CacheStatusSet, uint16_t, Unchanged = 0, Changed = 1, Erased = 2)

template <typename T>
class Set
{
public:
  class Iterator
  {
  public:
    Iterator(uint64_t _current_nonce, const Set& _set)
        : current_nonce(_current_nonce)
        , set(_set)
    {
    }
    T operator*()
    {
      if (current_nonce == 0)
        return T{};
      return set.get_node(current_nonce).val;
    }

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
  Set(const Hash& _hash)
      : key(_hash)
  {
    auto result = Global::get().m_ctx->store.get(key);
    if (result) {
      Buffer buf(gsl::as_bytes(gsl::make_span(*result)));
      buf >> nonce_node >> nonce_root >> m_size;
    } else {
      nonce_node = 0;
      nonce_root = 0;
      m_size = 0;
    }
  }

  ~Set()
  {
    if (Global::get().flush) {
      Buffer buf;
      buf << nonce_node << nonce_root << m_size;
      Global::get().m_ctx->store.put(key, buf.to_raw_string());

      for (auto& [id, node] : cache) {
        if (node.status == CacheStatusSet::Changed) {
          Global::get().m_ctx->store.put(sha256(key, id),
                                         Buffer::serialize<Node>(node));
        } else if (node.status == CacheStatusSet::Erased) {
          Global::get().m_ctx->store.del(sha256(key, id));
        }
      }
    }
  }

  bool insert(const T& val)
  {
    uint64_t before_insert_size = m_size;
    nonce_root = insert_node(nonce_root, val);
    return before_insert_size != m_size;
  }

  bool erase(const T& val)
  {
    uint64_t before_insert_size = m_size;
    nonce_root = delete_node(nonce_root, val);
    return before_insert_size != m_size;
    return 0;
  }

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

  uint64_t size() const
  {
    return m_size;
  }

  void pre_order(uint64_t current_nonce)
  {
    if (current_nonce != 0) {
      Node& current_node = get_node(current_nonce);
      NOCOMMIT_LOG("{} {} {} {} {} {}", current_nonce, current_node.left,
                   current_node.right, current_node.val, current_node.height,
                   current_node.parent);
      pre_order(current_node.left);
      pre_order(current_node.right);
    }
  }

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
  Node& get_node(uint64_t node_id)
  {
    if (auto it = cache.find(node_id); it != cache.end()) {
      return it->second;
    }

    auto result = Global::get().m_ctx->store.get(sha256(key, node_id));
    if (!result)
      throw Error("Node not found.");
    return cache.emplace(node_id, Buffer::deserialize<Node>(*result))
        .first->second;
  }

  const Node& get_node(uint64_t node_id) const
  {
    if (auto it = cache.find(node_id); it != cache.end()) {
      return it->second;
    }

    auto result = Global::get().m_ctx->store.get(sha256(key, node_id));
    if (!result)
      throw Error("Node not found.");
    return cache.emplace(node_id, Buffer::deserialize<Node>(*result))
        .first->second;
  }

  uint64_t new_node(const T& val)
  {
    nonce_node++;
    m_size++;
    cache.emplace(nonce_node, Node{0, 0, 1, 0, val, CacheStatusSet::Changed});
    return nonce_node;
  }

  uint64_t get_height(uint64_t node_nonce)
  {
    if (node_nonce == 0)
      return 0;
    return get_node(node_nonce).height;
  }

  uint64_t min_value_node(uint64_t root_nonce)
  {
    while (true) {
      Node& current_node = get_node(root_nonce);
      if (current_node.left == 0)
        return root_nonce;
      root_nonce = current_node.left;
    }
  }

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
      tmp.status = CacheStatusSet::Changed;
    }

    top_node.height =
        std::max(get_height(top_node.left), get_height(top_node.right)) + 1;
    left_node.height =
        std::max(get_height(left_node.left), get_height(left_node.right)) + 1;

    top_node.status = CacheStatusSet::Changed;
    left_node.status = CacheStatusSet::Changed;

    return x;
  }

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
      tmp.status = CacheStatusSet::Changed;
    }

    top_node.height =
        std::max(get_height(top_node.left), get_height(top_node.right)) + 1;
    right_node.height =
        std::max(get_height(right_node.left), get_height(right_node.right)) + 1;

    top_node.status = CacheStatusSet::Changed;
    right_node.status = CacheStatusSet::Changed;

    return y;
  }

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
    current_node.status = CacheStatusSet::Changed;
    // TODO balance tree

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
          current_node.status = CacheStatusSet::Changed;
          deleted_node_nonce = child;
        }

        // Delete node
        m_size--;
        Node& delete_node = get_node(deleted_node_nonce);
        delete_node.status = CacheStatusSet::Erased;
      } else {
        Node& min_node = get_node(min_value_node(current_node.right));

        current_node.val = min_node.val;
        current_node.right = delete_node(current_node.right, current_node.val);
      }
    }

    if (current_node.status == CacheStatusSet::Erased)
      return 0;

    current_node.height = 1 + std::max(get_height(current_node.left),
                                       get_height(current_node.right));
    current_node.status = CacheStatusSet::Changed;

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
  struct Node {
    uint64_t left;
    uint64_t right;
    uint64_t height;
    uint64_t parent;
    T val;
    CacheStatusSet status = CacheStatusSet::Unchanged;
  };

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

  const Hash key;
  uint64_t nonce_node;
  uint64_t nonce_root;
  uint64_t m_size;

  mutable std::unordered_map<uint64_t, Node> cache;
};
