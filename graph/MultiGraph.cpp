//
//  MultiGraph.cpp
//  Graphoffline
//
//  Created by Олег on 02.01.2020.
//

#include "MultiGraph.h"


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
