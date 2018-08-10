// #pragma once

// #include "tx/message.h"
// #include "util/bytes.h"

// class Output
// {
// public:
//   Output(Bytes<32> ident_)
//       : Output(ident_, {}, 0)
//   {
//   }
//   Output(Bytes<32> ident_, Bytes<20> owner_, uint64_t amount_)
//       : ident(ident_)
//       , owner(owner_)
//       , amount(amount_)
//   {
//   }

//   std::string to_string() const
//   {
//     return "Output[ident={}, owner={}, amount={}]"_format(ident, owner,
//     amount);
//   }

// public:
//   const Bytes<20> owner;
//   const uint64_t nonce = 0;
//   const uint64_t amount = 0;
// };
