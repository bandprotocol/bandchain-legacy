#include "merkle.h"

#include "crypto/sha256.h"

bool MerkleTree::add(std::unique_ptr<Hashable> data)
{
  Hash key = data->key();
  auto current = std::ref(root);

  for (size_t i = 0; i < Hash::Bits; ++i) {
    // TODO
    // bool bit_value = key.get_bit(i);
    bool bit_value = true;
    current = current.get()->children[bit_value];
    if (!current.get()) {
      current.get() = std::make_unique<MerkleNode>();
    }
  }

  auto& current_data = current.get()->data;
  if (current_data)
    return false;

  current_data = std::move(data);
  return true;
}

Hashable& MerkleTree::find_hashable(const Hash& key) const
{
  auto current = std::ref(root);

  for (size_t i = 0; i < Hash::Bits; ++i) {
    // TODO
    // bool bit_value = key.get_bit(i);
    bool bit_value = true;
    current = current.get()->children[bit_value];
    if (!current.get()) {
      return NIL;
    }
  }

  const auto& current_data = current.get()->data;
  if (current_data)
    return *current_data;

  return NIL;
}

Hash MerkleTree::MerkleNode::hash() const
{
  if (data)
    return data->hash();

  Hash hash0 = children[0] ? children[0]->hash() : Hash();
  Hash hash1 = children[1] ? children[1]->hash() : Hash();

  if (!hash0.is_empty() && !hash1.is_empty())
    return sha256(hash0 + hash1);

  if (!hash0.is_empty())
    return hash0;

  if (!hash1.is_empty())
    return hash1;

  return Hash();
}

std::string MerkleTree::MerkleNode::to_string() const
{
  if (data)
    return data->to_string();
  if (!children[0] && !children[1])
    return "";
  if (!children[0])
    return children[1]->to_string();
  if (!children[1])
    return children[0]->to_string();
  return children[0]->to_string() + "\n" + children[1]->to_string();
}
