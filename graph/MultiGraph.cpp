//
//  MultiGraph.cpp
//  Graphoffline
//
//  Created by Олег on 02.01.2020.
//

#include "MultiGraph.h"
#include "IAlgorithm.h"
#include "WeightGraph.h"

IndexType MultiGraph::GetEdgesNumber(ObjectId source, ObjectId target) const
{
    IndexType res = 0;
    NodePtr sourcePtr, targetPtr;
    if (IsValidNodeId(source, sourcePtr) && IsValidNodeId(target, targetPtr))
    {
        std::for_each(m_edges.begin(), m_edges.end(),
            [sourcePtr, targetPtr, &res](const EdgePtr & edge){
                if ((edge->source == sourcePtr && edge->target == targetPtr) ||
                    (edge->source == targetPtr && edge->target == sourcePtr && !edge->direct))
                {
                   res++;
                }
            });
    }
    
    return res;
}

ObjectId MultiGraph::GetEdge(ObjectId source, ObjectId target, const IndexType & index) const
{
    auto edge = FindEdge(source, target, index);
    if (edge)
    {
        return edge->privateId;
    }
    
    return ObjectId();
}

bool MultiGraph::GetEdgeStrId(ObjectId edge, char* outBuffer, IndexType bufferSize) const
{
    bool res = false;
    EdgePtr edgePtr;
    if (IsValidEdgeId(edge, edgePtr))
    {
        res = edgePtr->id.PrintLocale(outBuffer, bufferSize) > 0;
    }
    return res;
}

void MultiGraph::RemoveEdgeByID(ObjectId edgeId)
{
    Graph::RemoveEdge(GetEdgeById(edgeId));
}

Graph* MultiGraph::MakeGraphCopy(GraphCopyType type, const std::function<Graph*()> & createFunction) const
{
    if (type == GTC_MULTI_TO_COMMON_GRAPH_MINIMAL_EDGES)
    {
        return MakeCommonMinGraph(createFunction);
    }
    else
        return Graph::MakeGraphCopy(type, createFunction);
    
    return nullptr;
}

Graph* MultiGraph::MakeCommonMinGraph(const std::function<Graph*()> & createFunction) const
{
    Graph* res = GetEdgeWeightType() ==  WT_INT ? IntGraph().CreateGraph():
                                                  FloatGraph().CreateGraph();
    CopyPropertiesTo(res);
    
    // Create all nodes.
    for (NodePtr node : m_nodes)
    {
        res->AddNode(node->id, node->privateId, node->fake);
    }
    
    // Add only minimal edges
    for (int index1 = 0; index1 < GetNodesCount(); index1++)
    {
        for (int index2 = index1; index2 < GetNodesCount(); index2++)
        {
            auto findBestEdge = [&](auto node1, auto node2) -> EdgePtr
                {
                    IndexType numberOfEdges = GetEdgesNumber(node1, node2);
                    float minEdge    = (float)INFINITY_PATH_INT;
                    EdgePtr bestEdge = nullptr;
                    for (IndexType edgeIndex = 0; edgeIndex < numberOfEdges; edgeIndex++)
                    {
                        auto  edge   = FindEdge(node1, node2, edgeIndex);
                        float weight = edge->template GetWeight<FloatWeightType>();
                        if (weight < minEdge)
                        {
                            minEdge  = weight;
                            bestEdge = edge;
                        }
                    }
                    
                    return bestEdge;
                };

            ObjectId node1 = GetNode(index1);
            ObjectId node2 = GetNode(index2);

            auto edge1 = findBestEdge(node1, node2);
            auto edge2 = findBestEdge(node2, node1);
            
            if (edge1 == nullptr && edge2 == nullptr)
                continue;
            
            auto addEdge = [&res](auto & edge)
                {
                    res->AddEdge(edge->id,
                         edge->source->privateId,
                         edge->target->privateId,
                         edge->direct,
                         edge->weight,
                         edge->privateId);
                };
            
            if (edge1 == edge2)
            {
                addEdge(edge1);
            }
            else
            {
                if (edge1)
                    addEdge(edge1);
                if (edge2)
                    addEdge(edge2);
            }
        }
    }
    
    return res;
}

ObjectId MultiGraph::GetEdge(ObjectId source, ObjectId target) const
{
	return GetEdge(source, target, 0);
}