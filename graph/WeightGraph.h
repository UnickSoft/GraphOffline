//
//  WeightGraph.h
//  Graphoffline
//
//  Created by Олег on 26.12.2019.
//

#pragma once

#include <stdio.h>
#include "IGraph.h"
#include "Graph.h"

template<class WeightInterface, typename WeightTmplType> class WeightGraph : public Graph, public virtual WeightInterface
{
public:
    // Get Egde weight of int graph.
    virtual WeightTmplType GetEdgeWeight(ObjectId source, ObjectId target) const override
    {
        return Graph::GetEdgeWeight<WeightTmplType>(source, target);
    }
    // Create copy of graph.
    virtual WeightInterface* MakeCopy(GraphCopyType type) const override
    {
        return (WeightInterface*)MakeGraphCopy(type);
    }
    
protected:

    Graph* CreateGraph() const override
    {
        return new WeightGraph<WeightInterface, WeightTmplType>();
    }
};

using IntGraph   = WeightGraph<IGraphInt,   IntWeightType>;
using FloatGraph = WeightGraph<IGraphFloat, FloatWeightType>;
