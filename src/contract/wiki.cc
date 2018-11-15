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

#include "contract/wiki.h"

#include "store/global.h"

Wiki::Wiki(const Address& wiki_id)
    : Contract(wiki_id, ContractID::Wiki)
{
}

void Wiki::add_triple(std::string subject, std::string predicate,
                      std::string object)
{
  Global::get().m_ctx->graph.add_edge(subject, predicate, object, "");
}

void Wiki::delete_triple(std::string subject, std::string predicate,
                         std::string object)
{
  Global::get().m_ctx->graph.delete_edge(subject, predicate, object, "");
}

void Wiki::debug_create() const
{
  DEBUG(log, "wiki created at {} {}", m_addr, (void*)this);
}

void Wiki::debug_save() const
{
  DEBUG(log, "wiki saved at {} {}", m_addr, (void*)this);
}
