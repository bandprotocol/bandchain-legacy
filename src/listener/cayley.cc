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

#include "cayley.h"

CayleyListener::CayleyListener(GraphStoreCayley& _graph)
    : graph(_graph)
{
}

void CayleyListener::begin(const BlockMsg& blk) {}

void CayleyListener::commit(const BlockMsg& blk) {}

void CayleyListener::handleAddWikiEdge(const BlockMsg& blk,
                                       const HeaderMsg& hdr,
                                       const AddWikiEdgeMsg& msg,
                                       const AddWikiEdgeResponse& res)
{
  graph.add_edge(msg.subject.to_string(), msg.predicate.to_string(),
                 msg.object.to_string(), "");
}

void CayleyListener::handleRemoveWikiEdge(const BlockMsg& blk,
                                          const HeaderMsg& hdr,
                                          const RemoveWikiEdgeMsg& msg,
                                          const RemoveWikiEdgeResponse& res)
{
  graph.delete_edge(msg.subject.to_string(), msg.predicate.to_string(),
                    msg.object.to_string(), "");
}
