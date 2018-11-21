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

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <enum/enum.h>

#include "inc/essential.h"
#include "util/buffer.h"
#include "util/bytes.h"
#include "util/json.h"

#define BAND_MACRO_STRUCT_DECL(R, _, TYPE)                                     \
  BOOST_PP_TUPLE_ELEM(0, TYPE) BOOST_PP_TUPLE_ELEM(1, TYPE){};

#define BAND_MACRO_STRUCT_BUF_IN(R, BUF, TYPE)                                 \
  BUF >> BOOST_PP_TUPLE_ELEM(1, TYPE);

#define BAND_MACRO_STRUCT_BUF_OUT(R, BUF, TYPE)                                \
  BUF << BOOST_PP_TUPLE_ELEM(1, TYPE);

#define BAND_MACRO_STRUCT_TO_STRING(R, RET, TYPE)                              \
  RET += " {} = {},"_format(BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(1, TYPE)),  \
                            BOOST_PP_TUPLE_ELEM(1, TYPE));

#define BAND_MACRO_STRUCT_ABI(R, RET, TYPE)                                    \
  RET.push_back(BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(1, TYPE)) + ":"s +      \
                TypeID<BOOST_PP_TUPLE_ELEM(0, TYPE)>::name);

#define BAND_STRUCT_DECL(NAME, SEQ)                                            \
  struct NAME {                                                                \
                                                                               \
    BOOST_PP_SEQ_FOR_EACH(BAND_MACRO_STRUCT_DECL, _, SEQ)                      \
                                                                               \
    Buffer& _buffer_in(Buffer& buf)                                            \
    {                                                                          \
      BOOST_PP_SEQ_FOR_EACH(BAND_MACRO_STRUCT_BUF_IN, buf, SEQ)                \
      return buf;                                                              \
    }                                                                          \
    Buffer& _buffer_out(Buffer& buf) const                                     \
    {                                                                          \
      BOOST_PP_SEQ_FOR_EACH(BAND_MACRO_STRUCT_BUF_OUT, buf, SEQ)               \
      return buf;                                                              \
    }                                                                          \
    friend Buffer& operator>>(Buffer& buf, NAME& data)                         \
    {                                                                          \
      return data._buffer_in(buf);                                             \
    }                                                                          \
    friend Buffer& operator<<(Buffer& buf, const NAME& data)                   \
    {                                                                          \
      return data._buffer_out(buf);                                            \
    }                                                                          \
    std::string to_string() const                                              \
    {                                                                          \
      std::string ret;                                                         \
      ret += "{";                                                              \
      ret += " {}:"_format(BOOST_PP_STRINGIZE(NAME));                          \
      BOOST_PP_SEQ_FOR_EACH(BAND_MACRO_STRUCT_TO_STRING, ret, SEQ)             \
      ret.pop_back();                                                          \
      ret += " }";                                                             \
      return ret;                                                              \
    }                                                                          \
    static json interface()                                                    \
    {                                                                          \
      json ret = json::array();                                                \
      BOOST_PP_SEQ_FOR_EACH(BAND_MACRO_STRUCT_ABI, ret, SEQ)                   \
      return ret;                                                              \
    }                                                                          \
  };

#define BAND_EMPTY_STRUCT_DECL(NAME)                                           \
  struct NAME {                                                                \
    friend Buffer& operator>>(Buffer& buf, NAME& data)                         \
    {                                                                          \
      return buf;                                                              \
    }                                                                          \
    friend Buffer& operator<<(Buffer& buf, const NAME& data)                   \
    {                                                                          \
      return buf;                                                              \
    }                                                                          \
    std::string to_string() const                                              \
    {                                                                          \
      return "{{ {} }}"_format(BOOST_PP_STRINGIZE(NAME));                      \
    }                                                                          \
    static json interface()                                                    \
    {                                                                          \
      return json::array();                                                    \
    }                                                                          \
  };

#define STRUCT(NAME, ...)                                                      \
  BAND_STRUCT_DECL(NAME, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define MESSAGE(NAME, ...)                                                     \
  BAND_STRUCT_DECL(BOOST_PP_CAT(NAME, Msg),                                    \
                   BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define RESPONSE(NAME, ...)                                                    \
  BAND_STRUCT_DECL(BOOST_PP_CAT(NAME, Response),                               \
                   BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define NO_RESPONSE(NAME) BAND_EMPTY_STRUCT_DECL(BOOST_PP_CAT(NAME, Response))

#define BAND_MACRO_MESSAGE_FOR_EACH(FOR_EACH_MACRO)                            \
  BOOST_PP_SEQ_FOR_EACH(FOR_EACH_MACRO, _,                                     \
                        BAND_MACRO_MESSAGE_TYPES(BOOST_PP_VARIADIC_TO_SEQ))
