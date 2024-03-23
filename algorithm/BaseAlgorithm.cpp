/**
 *
 *  Interface for algorithm.
 *
 */


#include "BaseAlgorithm.h"
#include <vector>

std::vector<std::vector<bool> > BaseAlgorithm::GetAdjacencyMatrixBool(const IGraph& graph)
{
    std::vector<std::vector<bool> > res;
    
    res.resize(graph.GetNodesCount());
    
    auto nodesCount = graph.GetNodesCount();
    
    for (IndexType i = 0; i < nodesCount; i++)
    {
        res[i].resize(nodesCount);
        
        for (IndexType j = 0; j < nodesCount; j++)
        {
            auto s = graph.GetNode(i);
            auto f = graph.GetNode(j);
            if (graph.IsEgdeExists(s, f, false))
            {
                res[i][j] = true;
            }
            else
            {
                res[i][j] = 0;
            }
        }
    }
    
    return res;
}

std::vector<std::vector<IndexType>> BaseAlgorithm::GetReachabilityMatrix(const IGraph& graph)
{
    std::vector<std::vector<IndexType>> res;
    
    res.resize(graph.GetNodesCount());
    
    auto nodesCount = graph.GetNodesCount();
    
    for (IndexType i = 0; i < nodesCount; i++)
    {
        ObjectId nodeId    = graph.GetNode(i);
        auto childrenCount = graph.GetConnectedNodes(nodeId);

        res[i].reserve(childrenCount);
        
        for (IndexType j = 0; j < childrenCount; j++)
        {
            auto childNodeIndex = graph.GetConnectedNodeIndex(nodeId, j);
            res[i].push_back(childNodeIndex);
        }
    }
    
    return res;
}

