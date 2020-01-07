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

protected:
    // Dont use
    void RemoveEdge(ObjectId source, ObjectId target) final {}
};
