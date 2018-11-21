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

#include "inc/essential.h"
#include "listener/base.h"
#include "store/graph_cayley.h"
#include "util/msg.h"

/// Cayley listener to connect and manage graph database. This listener is
/// responsible for adding and removing edge in graph database.
class CayleyListener : public BaseListener
{
public:
  CayleyListener(GraphStoreCayley& _graph);

public:
  /// Load the current state of this listener. May involve block height now.
  void load() final {}

  /// Begin a new block. For now we save edge immidiately after executing msg.
  /// The next task is implementing transaction model.
  void begin(const BlockMsg& blk) final;

  /// Like begin(), it will add commit to save all edges in block to database.
  void commit(const BlockMsg& blk) final;

  /// Add edge to graph database
  void handleAddWikiEdge(const BlockMsg& blk,
                         const HeaderMsg& hdr,
                         const AddWikiEdgeMsg& msg,
                         const AddWikiEdgeResponse& res) final;

  /// Remove edge to graph database
  void handleRemoveWikiEdge(const BlockMsg& blk,
                            const HeaderMsg& hdr,
                            const RemoveWikiEdgeMsg& msg,
                            const RemoveWikiEdgeResponse& res) final;

private:
  GraphStoreCayley& graph;
};
