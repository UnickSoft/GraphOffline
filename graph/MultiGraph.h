//
//  MultiGraph.hpp
//  Graphoffline
//
//  Created by Олег on 02.01.2020.
//

#pragma once

#include <stdio.h>
#include "Graph.h"

/**
 * Main Graph class.
 */
class MultiGraph: public Graph, public virtual IMultiGraph
{
public:
    // Number of edges between nodes
    IndexType GetEdgesNumber(ObjectId source, ObjectId target) const override;
    // Return Id of edge.
    ObjectId GetEdge(ObjectId source, ObjectId target, const IndexType & index) const override;
    // Return edge string Id.
    bool GetEdgeStrId(ObjectId edge, char* outBuffer, IndexType bufferSize) const override;
    // Remove by id
    void RemoveEdgeByID(ObjectId edgeId) override;
    
    Graph* MakeGraphCopy(GraphCopyType type, const std::function<Graph*()> & createFunction = std::function<Graph*()>()) const override;
    
    // Make current graph undirected.
    Graph* MakeCommonMinGraph(const std::function<Graph*()> & createFunction) const;
    
    // Dont use
    void RemoveEdge(ObjectId source, ObjectId target) final {}
};
