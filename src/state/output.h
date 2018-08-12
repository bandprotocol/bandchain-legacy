#pragma once

class TxOutput
{
public:
  TxOutput() = default;
  TxOutput(const TxOutput& txo) = default;

private:
  // Address owner;   //< Who owns this TxOutput?
  // uint64_t amount; //< How much is this?
};
