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

#include "util/string.h"

#include <boost/algorithm/string.hpp>

template <int MAX_LENGTH, StringCase CASE>
String<MAX_LENGTH, CASE>::String(const std::string& _string)
    : rawdata(_string)
{
  validate();
}

template <int MAX_LENGTH, StringCase CASE>
std::string String<MAX_LENGTH, CASE>::to_string() const
{
  return rawdata;
}

template <int MAX_LENGTH, StringCase CASE>
void String<MAX_LENGTH, CASE>::validate()
{
  if (rawdata.length() > MAX_LENGTH) {
    throw Error(
        "String<{}, {}>::validate: String's length ({}) exceed maximum length.",
        MAX_LENGTH, CASE == StringCase::Sensitive ? "Sensitive" : "InSensitive",
        rawdata.length());
  }
  if (std::any_of(rawdata.begin(), rawdata.end(),
                  [](unsigned char c) { return !std::isprint(c); })) {
    throw Error(
        "String<{}, {}>::validate: rawdata contains unprintable characters",
        MAX_LENGTH,
        CASE == StringCase::Sensitive ? "Sensitive" : "InSensitive");
  }
  if (CASE == StringCase::InSensitive) {
    boost::algorithm::to_lower(rawdata);
  }
}

template class String<20, StringCase::InSensitive>;
