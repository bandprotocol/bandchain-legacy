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

// #include "band/msg.h"
// #include "band/txgen.h"
// #include "inc/essential.h"
// #include "util/equation.h"
// #include "util/json.h"

// class MsgTest : public CxxTest::TestSuite
// {
// public:
//   void test_msg_hdr()
//   {
//     MsgHdr hdr;
//     hdr.msgid = MsgID::Unset;
//     hdr.ts = 1530066733000;
//     hdr.vk = VerifyKey::rand();

//     Buffer buf;
//     TS_ASSERT_EQUALS(buf.size_bytes(), 0);

//     buf << hdr;
//     TS_ASSERT_EQUALS(buf.size_bytes(), 39);

//     MsgHdr hdr_read;
//     buf >> hdr_read;

//     TS_ASSERT_EQUALS(buf.size_bytes(), 0);
//     TS_ASSERT_EQUALS(hdr.msgid, hdr_read.msgid);
//     TS_ASSERT_EQUALS(hdr.ts, hdr_read.ts);
//     TS_ASSERT_EQUALS(hdr.vk, hdr_read.vk);
//   }

//   void test_msg_mint()
//   {
//     TS_ASSERT_EQUALS(MintMsg::ID, MsgID::Mint);

//     MintMsg msg;
//     msg.token_key = ContractID::rand();
//     msg.value = 1000'000'000'000'000;

//     Buffer buf;
//     TS_ASSERT_EQUALS(buf.size_bytes(), 0);

//     buf << msg;
//     TS_ASSERT_EQUALS(buf.size_bytes(), 28);

//     MintMsg msg_read;
//     buf >> msg_read;

//     TS_ASSERT_EQUALS(buf.size_bytes(), 0);
//     TS_ASSERT_EQUALS(msg.token_key, msg_read.token_key);
//     TS_ASSERT_EQUALS(msg.value, msg_read.value);
//   }

//   void test_msg_tx()
//   {
//     TS_ASSERT_EQUALS(TxMsg::ID, MsgID::Tx);

//     TxMsg msg;
//     msg.token_key = ContractID::rand();
//     msg.dest = AccountID::rand();
//     msg.value = 1000'000'000'000'000;

//     Buffer buf;
//     TS_ASSERT_EQUALS(buf.size_bytes(), 0);

//     buf << msg;
//     TS_ASSERT_EQUALS(buf.size_bytes(), 48);

//     TxMsg msg_read;
//     buf >> msg_read;

//     TS_ASSERT_EQUALS(buf.size_bytes(), 0);
//     TS_ASSERT_EQUALS(msg.token_key, msg_read.token_key);
//     TS_ASSERT_EQUALS(msg.dest, msg_read.dest);
//     TS_ASSERT_EQUALS(msg.value, msg_read.value);
//   }

//   void test_msg_create()
//   {
//     // std::string m =
//     //     R"foo({"expressions": ["ADD", "SUB", "MUL", "2", "EXP", "Supply",
//     //     "2", "MUL", "115", "Supply", "79"],
//     //             "max_supply": "20",
//     //             "spread_type": "1",
//     //             "spread_value": "4"})foo";
//     // json j = json::parse(m);

//     // // Create body msg of create_msg from  param "expression"
//     // std::string pa = txgen::process_createCC(j);

//     // // pa store real 00010002.... cannot read
//     // Buffer buf;
//     // buf << gsl::make_span(pa);

//     // // Get equation hex format (readable)
//     // auto x = buf.to_string();
//     // // get unique_ptr<Eq> from parse buffer
//     // auto ptr = Eq::parse(buf);

//     // // Dump equation to string
//     // Buffer tmp_buf;
//     // ptr->dump(tmp_buf);

//     // std::string eq_bin = "010203070206080107020307730801074f010414";

//     // // Hex represent equation
//     // TS_ASSERT_EQUALS(eq_bin, x);

//     // // make Bytes from hex representation
//     // auto raw = Bytes<20>::from_hex(eq_bin);

//     // buf << raw.as_span();
//     // Buffer tmp_buf2;
//     // ptr->dump(tmp_buf2);

//     // TS_ASSERT_EQUALS(tmp_buf.to_string(), tmp_buf2.to_string());
//   }
// };
