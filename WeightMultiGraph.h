//
//  WeightMultiGraph.h
//  Graphoffline
//
//  Created by Олег on 03.01.2020.
//

#pragma once

#include <stdio.h>
#include "IGraph.h"
#include "Graph.h"
#include "MultiGraph.h"

template<class WeightInterface, typename WeightTmplType> class WeightMultiGraph : public MultiGraph, public virtual WeightInterface
{
public:
    
    static WeightInterface* CreateGraph(const IGraph* graph)
    {
        const Graph* pGraph = dynamic_cast<const Graph*>(graph);
        return pGraph ? dynamic_cast<WeightMultiGraph*>(pGraph->MakeGraphCopy(GCT_COPY, &WeightMultiGraph::CreateGraphStatic)) : nullptr;
    }
    
    // Get Egde weight of int graph.
    virtual WeightTmplType GetEdgeWeight(ObjectId edgeId) const override
    {
        const auto edge = GetEdgeById(edgeId);
        return edge ? edge->template GetWeight<WeightTmplType>() : WeightTmplType();
    }
    // Create copy of graph.
    virtual WeightInterface* MakeCopy(GraphCopyType type) const override
    {
        return (WeightInterface*)MakeGraphCopy(type);
    }
    
protected:

    Graph* CreateGraph() const override
    {
        return CreateGraphStatic();
    }
    
    static Graph* CreateGraphStatic()
    {
        return new WeightMultiGraph<WeightInterface, WeightTmplType>();
    }
};

using IntMultiGraph   = WeightMultiGraph<IMultiGraphInt,   IntWeightType>;
using FloatMultiGraph = WeightMultiGraph<IMultiGraphFloat, FloatWeightType>;
