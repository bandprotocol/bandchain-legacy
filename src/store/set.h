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
  Set(const Hash& _hash)
      : key(_hash)
  {
    auto result = Global::get().m_ctx->store.get(key);
    if (result) {
      Buffer buf{gsl::make_span(*result)};
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

  bool insert(T val)
  {
    if (m_size == 0) {
      nonce_node++;
      m_size++;
      cache.emplace(nonce_node, Node{0, 0, 0, val, CacheStatusSet::Changed});
      nonce_root = nonce_node;
      return true;
    }

    Node& root_node = get_node(nonce_root);
    if (val < root_node.val) {
      nonce_node++;
      m_size++;
      cache.emplace(nonce_node,
                    Node{0, nonce_root, 0, val, CacheStatusSet::Changed});
      root_node.parent = nonce_node;
      root_node.status = CacheStatusSet::Changed;
      nonce_root = nonce_node;
      return true;
    }

    uint256_t current_nonce_node = nonce_root;
    while (true) {
      Node& current_node = get_node(current_nonce_node);
      if (val == current_node.val) {
        return false;
      }
      if (val < current_node.val) {
        nonce_node++;
        m_size++;
        // Create new node
        cache.emplace(nonce_node,
                      Node{0, current_nonce_node, current_node.parent, val,
                           CacheStatusSet::Changed});
        // Set parent 's right to new node
        if (current_node.parent) {
          Node& parent_node = get_node(current_node.parent);
          parent_node.right = nonce_node;
          parent_node.status = CacheStatusSet::Changed;
        }

        // Set right node's parent to new node
        current_node.parent = nonce_node;
        current_node.status = CacheStatusSet::Changed;
        return true;
      }
      if (current_node.right == 0) {
        nonce_node++;
        m_size++;

        cache.emplace(nonce_node, Node{0, 0, current_nonce_node, val,
                                       CacheStatusSet::Changed});
        current_node.right = nonce_node;
        current_node.status = CacheStatusSet::Changed;
        return true;
      }
      current_nonce_node = current_node.right;
    }
  }

  bool erase(T val)
  {
    if (m_size == 0)
      return false;

    Node& root_node = get_node(nonce_root);
    if (val == root_node.val) {
      m_size--;
      if (root_node.right == 0) {
        root_node.status = CacheStatusSet::Erased;
        nonce_root = 0;
      } else {
        Node& right_node = get_node(root_node.right);
        root_node.status = CacheStatusSet::Erased;

        nonce_root = root_node.right;
        right_node.parent = 0;
        right_node.status = CacheStatusSet::Changed;
      }
      return true;
    }

    uint256_t current_nonce_node = nonce_root;
    while (true) {
      Node& current_node = get_node(current_nonce_node);
      if (val == current_node.val) {
        // Change right of parent node to right of this one
        Node& parent_node = get_node(current_node.parent);
        parent_node.right = current_node.right;
        parent_node.status = CacheStatusSet::Changed;

        // Change parent of right node to parent of this one
        if (current_node.right) {
          Node& right_node = get_node(current_node.right);
          right_node.parent = current_node.parent;
          right_node.status = CacheStatusSet::Changed;
        }

        current_node.status = CacheStatusSet::Erased;
        m_size--;

        return true;
      }
      if (val < current_node.val) {
        return false;
      }
      // It's a last node.
      if (current_node.right == 0) {
        return false;
      }
      current_nonce_node = current_node.right;
    }
  }

  bool contains(T val)
  {
    if (m_size == 0)
      return false;
    uint256_t current_nonce_node = nonce_root;
    while (true) {
      Node& current_node = get_node(current_nonce_node);
      if (val == current_node.val) {
        return true;
      }
      if (val < current_node.val) {
        return false;
      }
      // It's a last node.
      if (current_node.right == 0) {
        return false;
      }
      current_nonce_node = current_node.right;
    }
  }

  T get_max()
  {
    if (m_size == 0)
      return T{};
    uint256_t current_nonce_node = nonce_root;
    while (true) {
      Node& current_node = get_node(current_nonce_node);
      if (current_node.right == 0) {
        return current_node.val;
      }
      current_nonce_node = current_node.right;
    }
  }

  uint256_t size() { return m_size; }

private:
  struct Node;
  Node& get_node(uint256_t node_id)
  {
    if (auto it = cache.find(node_id); it != cache.end()) {
      return it->second;
    }

    auto result = Global::get().m_ctx->store.get(sha256(key, node_id));
    if (!result)
      throw("Node not found.");
    return cache.emplace(node_id, Buffer::deserialize<Node>(*result))
        .first->second;
  }

private:
  struct Node {
    uint256_t left;
    uint256_t right;
    uint256_t parent;
    T val;
    CacheStatusSet status = CacheStatusSet::Unchanged;
  };

  friend Buffer& operator<<(Buffer& buf, const Node& node)
  {
    return buf << node.left << node.right << node.parent << node.val;
  }

  friend Buffer& operator>>(Buffer& buf, Node& node)
  {
    return buf >> node.left >> node.right >> node.parent >> node.val;
  }

  const Hash key;
  uint256_t nonce_node;
  uint256_t nonce_root;
  uint256_t m_size;

  std::unordered_map<uint256_t, Node> cache;
};
