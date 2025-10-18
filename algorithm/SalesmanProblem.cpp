//
//  SalesmanProblem.cpp
//  Graphoffline
//
//  Created by Олег on 07.10.25.
//
//

#include "SalesmanProblem.h"
#include "IAlgorithmFactory.h"
#include <list>
#include <string.h>
#include <algorithm>
#include "Logger.h"
#include <vector>
#include <unordered_set>
#include <cassert>
#include <iostream>

#ifdef _MSC_VER
#undef isfinite
#undef fpclassify
#endif

#include <cmath>

static const char* g_indexStr = "index";
static const char* g_indexCountStr = "indexCount";

template<typename WeightType, typename GraphInterface>
bool CalculateResult(const GraphInterface &graph, WeightType& result, bool isPath, ObjectId startPath, std::vector<ObjectId>& path)
{
    const int n = graph.GetNodesCount();
    if (n == 0) return false;

    std::vector<ObjectId> nodes(n);
    int startNodeIndex = 0;
    for (int i = 0; i < n; ++i) 
    {
        nodes[i] = graph.GetNode(i);
        if (isPath && nodes[i] == startPath)
        {
            startNodeIndex = i;
        }
    }

    constexpr WeightType INF = std::numeric_limits<WeightType>::max();

    // Build directed distance matrix: make sure we require an edge from i -> j.
    std::vector<std::vector<WeightType>> dist(n, std::vector<WeightType>(n, INF));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            // Use IsEgdeExists to ensure direction i->j really exists (see IGraph API)
            if (graph.AreNodesConnected(nodes[i], nodes[j])) {
                dist[i][j] = graph.GetEdgeWeight(nodes[i], nodes[j]);
            }
        }
    }

    const size_t FULL_MASK = (1ULL << n);

    WeightType globalBest = INF;
    std::vector<ObjectId> bestPath;

    // Try every node as a starting point
    // For path variant, only try the specified start node
    for (int start = startNodeIndex; start < (isPath ? startNodeIndex + 1: n); ++start)
    {
        // dp[mask][v] = best cost to reach v after visiting 'mask'
        std::vector<std::vector<WeightType>> dp(FULL_MASK, std::vector<WeightType>(n, INF));
        std::vector<std::vector<int>> parent(FULL_MASK, std::vector<int>(n, -1));

        dp[1ULL << start][start] = static_cast<WeightType>(0);

        // DP transitions
        for (size_t mask = 0; mask < FULL_MASK; ++mask) {
            for (int u = 0; u < n; ++u) {
                if (!(mask & (1ULL << u))) continue;              // u not in mask
                if (dp[mask][u] == INF) continue;                 // unreachable state -> skip
                for (int v = 0; v < n; ++v) {
                    if (mask & (1ULL << v)) continue;             // v already visited
                    if (dist[u][v] == INF) continue;             // no edge u->v

                    // avoid overflow for unsigned types: ensure dp[mask][u] + dist[u][v] won't wrap
                    if constexpr (std::is_unsigned<WeightType>::value) {
                        if (dp[mask][u] > INF - dist[u][v]) continue;
                    }

                    size_t nextMask = mask | (1ULL << v);
                    WeightType newCost = dp[mask][u] + dist[u][v];
                    if (newCost < dp[nextMask][v]) {
                        dp[nextMask][v] = newCost;
                        parent[nextMask][v] = u;
                    }
                }
            }
        }

        // Try closing the cycle back to the start node
        WeightType bestCost = INF;
        int lastNode = -1;
        for (int i = 0; i < n; ++i) {
            if (i == start) continue;
            if (dp[FULL_MASK - 1][i] == INF) continue;         // must have visited all nodes
            if (dist[i][start] == INF) continue;               // must be able to return to start

            // avoid overflow check again for unsigned
            if constexpr (std::is_unsigned<WeightType>::value) {
                if (dp[FULL_MASK - 1][i] > INF - dist[i][start]) continue;
            }

            WeightType cost = dp[FULL_MASK - 1][i] + dist[i][start];
            if (cost < bestCost) {
                bestCost = cost;
                lastNode = i;
            }
        }

        if (bestCost == INF) continue; // no Hamiltonian cycle starting at this node

        // Reconstruct path from 'start' -> ... -> lastNode
        std::vector<int> order;               // will contain [start, ..., lastNode]
        size_t mask = FULL_MASK - 1;
        int curr = lastNode;
        while (curr != -1) {
            order.push_back(curr);
            int prev = parent[mask][curr];
            mask ^= (1ULL << curr);
            curr = prev;
        }
        std::reverse(order.begin(), order.end()); // now order[0] == start

        // optional: close the cycle by pushing start again at the end
        order.push_back(order.front()); // now length == n+1, cycle closed

        // sanity check: order should contain n+1 nodes and first==last
        if ((int)order.size() != n + 1) continue;

        // Convert indices to ObjectIds
        std::vector<ObjectId> candidatePath;
        candidatePath.reserve(order.size());
        for (int idx : order) candidatePath.push_back(nodes[idx]);

        if (bestCost < globalBest) {
            globalBest = bestCost;
            bestPath = std::move(candidatePath);
        }
    }

    if (globalBest == INF) return false; // no Hamiltonian cycle found

    result = globalBest;
    for (int i = 0; i < bestPath.size() - 1; ++i)
    {
        // Skip fake nodes in path output
        if (graph.IsFakeNode(bestPath[i]))
        {
            continue;
        }
        path.push_back(bestPath[i]);
        if (graph.IsFakeNode(bestPath[i + 1]))
        {
            continue;
        }
        path.push_back(graph.GetEdge(bestPath[i], bestPath[i + 1]));
    }
    if (!isPath)
    {
        path.push_back(bestPath.back());
    }
    return true;
}


SalesmanProblem::SalesmanProblem (bool isFloat, bool isPath) : 
    m_isFloat(isFloat), m_isPath(isPath)
{
    
}

SalesmanProblem::~SalesmanProblem ()
{
    
}

// Calculate algorithm.
bool SalesmanProblem::Calculate()
{
    m_bResult = false;
    
    GraphPtr pGraph = GraphPtr(m_pGraph->MakeBaseCopy(GCT_COPY));
    /* Out of memory */
    if (!pGraph)
    {
        return false;
    }
    
    AlgorithmPtr connectedComponent = AlgorithmPtr(m_pAlgorithmFactory->CreateAlgorithm("concomp", pGraph.get()));

    assert(connectedComponent);
    /* Can find algorithm */
    if (!connectedComponent)
    {
        return false;
    }
            
    connectedComponent->Calculate();
    /* Vertexes are not connected */
    if (connectedComponent->GetResult(0).nValue > 1)
    {
        return false;
    }
        

    const int n = m_pGraph->GetNodesCount();
    if (n == 0)
    {
        m_bResult = true;
        return true;
    }

    if (n == 1)
    { 
        m_path.push_back(m_pGraph->GetNode((IndexType)0));
        m_bResult = true;
        return true;
    }

    /* Graph os too large for this algorithm */
    if (n > 30)
    { 
        return false;
    }

    GraphPtr pGraphNonMulti = m_pGraph->IsMultiGraph() ? GraphPtr(m_pGraph->MakeBaseCopy(GTC_MULTI_TO_COMMON_GRAPH_MINIMAL_EDGES)) : 
                                                         pGraph;

    // Add fake node to search path using loop algorithm.
    if (m_isPath)
    {
        auto fakeNodeId = pGraphNonMulti->AddNode(true); // fake node
        if (m_startNode == 0)
        {
            m_startNode = pGraphNonMulti->GetNode((IndexType)0);
        }

        pGraphNonMulti->AddEdge(fakeNodeId, m_startNode, true, 0.0);

        for (int i = 0; i < pGraphNonMulti->GetNodesCount(); ++i)
        {
            auto nodeId = pGraphNonMulti->GetNode(i);
            if (nodeId != m_startNode)
            {
                // Connect all other nodes with fake node.
                pGraphNonMulti->AddEdge(nodeId, fakeNodeId, true, 0.0);
            }
        }
    }

    if (m_isFloat)
    {
        IGraphFloat* pFloatGraph = dynamic_cast<IGraphFloat*>(pGraphNonMulti.get());
        m_bResult = CalculateResult<float>(*pFloatGraph, m_minCostFloat, m_isPath, m_startNode, m_path);
    }
    else
    {
        IGraphInt* pIntGraph = dynamic_cast<IGraphInt*>(pGraphNonMulti.get());
        m_bResult = CalculateResult<int32_t>(*pIntGraph, m_minCostInt, m_isPath, m_startNode, m_path);
    }
    if (m_bResult)
    {
         if (m_isPath && m_path.size() >= 2)
         {
             //m_path.pop_back(); // Start from the end;
         }
    }

    return m_bResult;
}

// Hightlight nodes count.
IndexType SalesmanProblem::GetHightlightNodesCount() const
{
    if (m_path.empty())
    {
        return 0;
    }
    
    // If path
    return (IndexType)(m_isPath ? m_path.size() / 2 + 1 : m_path.size() / 2);
}

// Hightlight node.
ObjectId SalesmanProblem::GetHightlightNode(IndexType index) const
{
    return m_path[2 * index];
}

// Hightlight edges count.
IndexType SalesmanProblem::GetHightlightEdgesCount() const
{
    if (m_path.empty())
    {
        return 0;
    }

    bool use_same_edge_twise = m_path.size() == 5 && !m_isPath && m_path[1] == m_path[3];
    
    return (IndexType)(m_path.size() / 2) + (use_same_edge_twise ? -1 : 0);
}

// Hightlight edge.
NodesEdge SalesmanProblem::GetHightlightEdge(IndexType index) const
{
    NodesEdge res;
    res.source = m_path[2 * index];
    res.target = m_path[2 * index + 2];
    res.edgeId = m_path[2 * index + 1];
    return res;
}

// Get result count.
IndexType SalesmanProblem::GetResultCount() const
{
    return (IndexType)(2 + m_path.size());
}

// Get result.
AlgorithmResult SalesmanProblem::GetResult(IndexType index) const
{
    AlgorithmResult result;
    
    constexpr int path_results_offset = 2;
    if (index == 0)
    {
        result = AlgorithmResult((IntWeightType)m_bResult);
    }
    else if (index == 1)
    {
        if (m_isFloat)
        {
            result = AlgorithmResult((FloatWeightType)m_minCostFloat);
        }
        else
        {
            result = AlgorithmResult((IntWeightType)m_minCostInt);
        }
    }
    else if (index < m_path.size() + path_results_offset)
    {
        if ((index - path_results_offset) % 2 == 0)
        {
            result.type = ART_NODES_PATH;
            m_pGraph->GetNodeStrId(m_path[index - path_results_offset], result.strValue,
                                sizeof(result.strValue));
        }
        else
        {
            result.type = ART_EDGES_PATH;
            m_pGraph->GetEdgeStrId(m_path[index - path_results_offset], result.strValue,
                                sizeof(result.strValue));
        }
    }
    
    return result;
}

// Get propery
bool SalesmanProblem::GetNodeProperty(ObjectId object, IndexType index, AlgorithmResult* param) const
{
    bool res = false;
    if (index == 0 && param)
    {
        auto position = std::find(m_path.begin(), m_path.end(), object);
        
        if (position != m_path.end())
        {
            param->type = ART_INT;
            param->nValue = (IntWeightType)(position - m_path.begin());
            res = true;
        }
    }
    return res;
}

const char* SalesmanProblem::GetNodePropertyName(IndexType index) const
{
    if (index == 0)
    {
        return g_indexCountStr;
    }
    else if (index == 1)
    {
        return g_indexStr;
    }
    
    return nullptr;
}

bool SalesmanProblem::EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const
{
    bool res = false;
    
    switch (index)
    {
        case 0:
        {
            strncpy(outParamInfo->paramName, "start", sizeof(outParamInfo->paramName));
            outParamInfo->type = APT_NODE;
            res = true;
            break;
        }
    }
    
    return res;
}

void SalesmanProblem::SetParameter(const AlgorithmParam* param)
{
    if (param)
    {
        if (strncmp(param->paramName, "start", sizeof(AlgorithmParam().paramName)) == 0)
        {
            m_startNode = param->data.id;
        }
    }
}