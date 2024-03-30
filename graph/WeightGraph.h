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
    
public:

    Graph* CreateGraph() const override
    {
        return new WeightGraph<WeightInterface, WeightTmplType>();
    }
    
    virtual Graph* MakeGraphRemoveNegative(const std::function<Graph*()> & createFunction) const override
    {
        WeightGraph<WeightInterface, WeightTmplType>* res = new WeightGraph<WeightInterface, WeightTmplType>();

        CopyPropertiesTo(res);

        // Create all nodes.
        for (NodePtr node : m_nodes)
        {
            res->m_nodes.push_back(NodePtr(new Node(node->id, node->privateId, node->fake, node->index)));
            res->m_idToNode[node->privateId] = res->m_nodes.back();
        }

        // Add edges.
        for (EdgePtr edge : m_edges)
        {
            if (edge->GetWeight<WeightTmplType>() >= (WeightTmplType)0)
            {
                res->AddEdge(edge->id,
                         edge->source->privateId,
                         edge->target->privateId,
                         edge->direct,
                         edge->weight,
                         edge->privateId);
            }
        }

        return res;
    }
};

using IntGraph   = WeightGraph<IGraphInt,   IntWeightType>;
using FloatGraph = WeightGraph<IGraphFloat, FloatWeightType>;
