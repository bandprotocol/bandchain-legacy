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

// #include <cxxtest/TestSuite.h>
// #include <vector>

// #include "inc/essential.h"
// #include "util/iban.h"

// class IBANTest : public CxxTest::TestSuite
// {
// public:
//   void testParseFromAddress()
//   {
//     auto addr =
//     Address::from_hex("269e974f44b178baa444db07e5726de74a5b8679");
//     TS_ASSERT_EQUALS(
//         AccountID::from_addr(addr).to_string(),
//         std::string("AX02 E4RK QV4E YF6M XKCE 5ND8 L6VP 67FF ZBV3"));
//     TS_ASSERT_EQUALS(
//         ContractID::from_addr(addr).to_string(),
//         std::string("CX81 E4RK QV4E YF6M XKCE 5ND8 L6VP 67FF ZBV3"));
//     addr = Address::from_hex("0000000000000000000000000000000000000000");
//     TS_ASSERT_EQUALS(
//         RevenueID::from_addr(addr).to_string(),
//         std::string("RX16 AAAA AAAA AAAA AAAA AAAA AAAA AAAA AAAA"));
//   }

//   void testParseAccountFromIBAN()
//   {
//     TS_ASSERT_EQUALS(
//         AccountID::from_string("AX02 E4RK QV4E YF6M XKCE 5ND8 L6VP 67FF
//         ZBV3"),
//         AccountID::from_hex("269e974f44b178baa444db07e5726de74a5b8679"));
//   }

//   void testParseContractFromIBAN()
//   {
//     TS_ASSERT_EQUALS(
//         ContractID::from_string("CX81 E4RK QV4E YF6M XKCE 5ND8 L6VP 67FF
//         ZBV3"),
//         ContractID::from_hex("269e974f44b178baa444db07e5726de74a5b8679"));
//   }

//   void testBandToken()
//   {
//     TS_ASSERT_EQUALS(
//         ContractID().to_string(),
//         std::string("CX54 AAAA AAAA AAAA AAAA AAAA AAAA AAAA AAAA"));

//     TS_ASSERT_EQUALS(
//         ContractID::from_string("CX54 AAAA AAAA AAAA AAAA AAAA AAAA AAAA
//         AAAA"), ContractID());

//     TS_ASSERT_EQUALS(
//         RevenueID::from_string("RX70 RRRR RRRR RRRR RRRR RRRR RRRR RRRR
//         RRRR"),
//         RevenueID::from_hex("7bdef7bdef7bdef7bdef7bdef7bdef7bdef7bdef"));

//     TS_ASSERT_EQUALS(
//         ContractID::from_string("CX09 CCCC CCCC CCCC CCCC CCCC CCCC CCCC
//         CCCC"),
//         ContractID::from_hex("1084210842108421084210842108421084210842"));

//     TS_ASSERT_EQUALS(
//         AccountID::from_string("AX72 AAAA AAAA AAAA AAAA AAAA AAAA AAAA
//         AAAA"),
//         AccountID::from_hex("0000000000000000000000000000000000000000"));

//     TS_ASSERT_EQUALS(
//         StakeID::from_string("SX87 SSSS SSSS SSSS SSSS SSSS SSSS SSSS SSSS"),
//         StakeID::from_hex("8421084210842108421084210842108421084210"));
//   }
// };
