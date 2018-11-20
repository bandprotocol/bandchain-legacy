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

#include <boost/preprocessor/comparison/greater.hpp>
#include <boost/preprocessor/logical/not.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/variadic/size.hpp>

#include "inc/essential.h"
#include "util/equation.h"
#include "util/msgimpl.h"
#include "util/string.h"

// clang-format off

/// Macro to define all messages available in Band Protocol. When you add a
/// new message type, make sure to add it at the bottom so it won't ruin the
/// existing messages.

#define BAND_MACRO_MESSAGE_TYPES(MACRO, ...)                                   \
  MACRO(__VA_ARGS__                                                            \
        BOOST_PP_COMMA_IF(                                                     \
            BOOST_PP_GREATER(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 1))          \
        CreateAccount,                                                         \
        CreateToken,                                                           \
        MintToken,                                                             \
        TransferToken,                                                         \
        BuyToken,                                                              \
        SellToken                                                              \
  )
// clang-format on

/// Convenient macro to prefix the given argument with "handle".
#define BAND_MACRO_HANDLE(NAME) BOOST_PP_CAT(handle, NAME)

/// Convenient macro to suffix the given argument with "Msg".
#define BAND_MACRO_MSG(NAME) BOOST_PP_CAT(NAME, Msg)

/// Convenient macro to suffix the given argument with "Response".
#define BAND_MACRO_RESPONSE(NAME) BOOST_PP_CAT(NAME, Response)

/// Declare ENUM to represent each of the message types.
BAND_MACRO_MESSAGE_TYPES(ENUM, MsgType, uint16_t)

STRUCT(
    /// Block Message gives the summary of a block. Each block contains one
    /// or more transactions.
    BlockMsg,
    (uint64_t, height),   //< The height of this block
    (uint64_t, timestamp) //< The time of the block
)

STRUCT(
    /// Header Message is a part of the message the is common to all message
    /// types. Note that some of the fields are not passed by the clients but
    /// rather computed internally.
    HeaderMsg,         //<
    (Ident, user),     //< The ID of the user sending this tranaction
    (Signature, sig),  //< The signature of this transaction
    (uint64_t, nonce), //< Strictly increasing sequence of the sender
    (Hash, hash)       //< The hash of the whole message
)

////////////////////////////////////////////////////////////////////////////////
/// Meessages //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MESSAGE(
    /// CreateAccount Message allows an existing account to create another
    /// account, given the necessary condition.
    CreateAccount,   //<
    (PublicKey, pk), //< Ed25519 public key of the created account
    (Ident, user)    //< Human-readable user identifier
)

MESSAGE(
    /// CreateToken Message allows anyone to create a token bonded with an
    /// existing tokens (e.g. Band) through a pre-defined curve.
    CreateToken,           //<
    (Ident, baseToken),    //< The ID of the base token
    (Ident, createdToken), //< The ID of the newly created token
    (Curve, curve)         //< Bonding-curve of the created token
)

MESSAGE(
    /// MintToken Message allows anyone to mint any token they wish. It is
    /// clearly for testing purpose only and will not be available on mainnet.
    MintToken,         //<
    (Ident, token),    //< The ID of the token contract
    (uint256_t, value) //< The amount of tokens to mint
)

MESSAGE(
    /// TransferToken Message allows anyone to send their tokens to another
    /// Band account. The sender must have enough fund or the transaction would
    /// get rejected.
    TransferToken,     //<
    (Ident, token),    //< The ID of the token contract
    (Ident, dest),     //< The user ID to send tokens to
    (uint256_t, value) //< The amount of tokens to send
)

MESSAGE(
    /// BuyToken Message allows anyone to buy some tokens by paying the base
    /// tokens. The transaction would get rejected if the user does not have
    /// enough base tokens.
    BuyToken,          //<
    (Ident, token),    //< The ID of the token contract
    (uint256_t, value) //< The amount of new tokens to buy
)
MESSAGE(
    /// SellToken Message allows anyone to sell some tokens to gain the base
    /// tokens. The transaction would get rejected if the user does not have
    /// enough tokens to sell.
    SellToken,         //<
    (Ident, token),    //< The ID of the token contract
    (uint256_t, value) //< The amount of tokens to sell
)

////////////////////////////////////////////////////////////////////////////////
/// Responses //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
NO_RESPONSE(CreateAccount)
NO_RESPONSE(CreateToken)
NO_RESPONSE(MintToken)
NO_RESPONSE(TransferToken)

RESPONSE(
    /// BuyToken Response contains the number of base tokens that was spent
    /// in the process.
    BuyToken,
    (Ident, baseToken), //< Base token
    (uint256_t, spent)  //< The amount of base tokens spent
)

RESPONSE(
    /// SellToken Response contains the number of base tokens the sender
    /// received after completing the sell-back.
    SellToken,
    (Ident, baseToken),   //< Base token
    (uint256_t, received) //< The amount of base tokens received
)
