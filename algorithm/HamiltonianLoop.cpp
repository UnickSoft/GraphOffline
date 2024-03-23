//
//  HamiltonianLoop.cpp
//  Graphoffline
//
//  Created by Олег on 26.12.15.
//
//

#include "HamiltonianLoop.h"
#include "IAlgorithmFactory.h"
#include <list>
#include <string.h>
#include <algorithm>
#include "Logger.h"
#include <vector>
#include <unordered_set>
#include <cassert>

static const char* g_indexStr = "index";
static const char* g_indexCountStr = "indexCount";

HamiltonianLoop::HamiltonianLoop (bool path) : m_path(path), m_bResult(false)
{
    
}

HamiltonianLoop::~HamiltonianLoop ()
{
    
}


// Calculate algorithm.
bool HamiltonianLoop::Calculate()
{
    m_bResult = false;
    
    const IGraph* currentGraph = m_pGraph;
    GraphPtr scopeGraph;
    ObjectId addedNode;
    
    if (m_path)
    {
        scopeGraph   = GraphPtr(m_pGraph->MakeBaseCopy(GCT_COPY));
        currentGraph = scopeGraph.get();
        addedNode    = scopeGraph->AddNode(true);
        
        for (int i = 0; i < scopeGraph->GetNodesCount() - 1; i++)
        {
            scopeGraph->AddEdge(addedNode, scopeGraph->GetNode(i), false, 1);
        }
    }
    
    AlgorithmPtr connectedComponent = AlgorithmPtr(m_pAlgorithmFactory->CreateAlgorithm("concomp", currentGraph));
    
    if (connectedComponent)
    {
        bool bCanHasLoop   = false;
        
        std::vector<ObjectId> odd_vertex;
        
        // Find in full directed graph.
        if (currentGraph->HasDirected())
        {
            AlgorithmParam param;
            strncpy(param.paramName, "strong", sizeof(param.paramName));
            param.data.bFlag = true;
            
            //Search strong connected component.
            connectedComponent->SetParameter(&param);
            
            connectedComponent->Calculate();
            IntWeightType componentCount = connectedComponent->GetResult(0).nValue;
            
            bCanHasLoop = (componentCount == 1);
        } // Find in full undirected graph.
        else if (!currentGraph->HasDirected())
        {
            connectedComponent->Calculate();
            IntWeightType componentCount = connectedComponent->GetResult(0).nValue;
            
            bCanHasLoop = (componentCount == 1) &&
              (m_path || m_pGraph->GetNodesCount() != 2 || m_pGraph->IsMultiGraph()); // Fix K2 case https://www.reddit.com/r/math/comments/7404nz/is_the_complete_graph_on_two_vertices_k2/?utm_source=amp&utm_medium=&utm_content=comments_view_all
        }

        if (bCanHasLoop)
        {
            std::vector<std::vector<IndexType>> reachabilityMatrix = GetReachabilityMatrix(*currentGraph);
    
            int nodesCount = currentGraph->GetNodesCount();

            std::vector<int> path;
            path.resize(nodesCount, -1);

            std::vector<int> step;
            step.resize(nodesCount, -1);
            
            m_startNode = m_path ? nodesCount - 1 : 0;
            
            path[0] = m_startNode;
            step[m_startNode] = m_startNode;
            
            m_bResult = _FindHamiltonianLoopRecursive(1, reachabilityMatrix, path, step);
            
            if (nodesCount == 1)
            {
                m_bResult = true;
            }
            
            if (m_bResult)
            {
                int realNodesCount = m_pGraph->GetNodesCount();
                
                if (m_path)
                {
                    m_HamiltonianLoop.resize(realNodesCount + (realNodesCount - 1));
                
                    int elementNum = 0;
                    bool first = true;
                    int prevNode = -1;
                    for (int index : path)
                    {
                        if (first)
                        {
                            // Add vertex
                            first = false;
                            continue;
                        }
                        
                        if (prevNode != -1)
                        {
                            // Add edge
                            m_HamiltonianLoop[elementNum++] = currentGraph->GetEdge(currentGraph->GetNode(prevNode), currentGraph->GetNode(index));
                        }
                        // Add vertex
                        m_HamiltonianLoop[elementNum++] = currentGraph->GetNode(index);
                        prevNode = index;
                    }
                    
                    m_bResult = (elementNum == realNodesCount + (realNodesCount - 1));
                }
                else // loop
                {
                    FillLoop(path, currentGraph, nodesCount);
                }
            }
        }
    }

    return true;
}

// Hightlight nodes count.
IndexType HamiltonianLoop::GetHightlightNodesCount() const
{
    if (m_HamiltonianLoop.empty())
    {
        return 0;
    }
    
    return (IndexType)(m_path ? m_HamiltonianLoop.size() / 2 + 1 : m_HamiltonianLoop.size() / 2);
}

// Hightlight node.
ObjectId HamiltonianLoop::GetHightlightNode(IndexType index) const
{
    return m_HamiltonianLoop[2 * index];
}

// Hightlight edges count.
IndexType HamiltonianLoop::GetHightlightEdgesCount() const
{
    if (m_HamiltonianLoop.empty())
    {
        return 0;
    }
    
    return (IndexType)(m_HamiltonianLoop.size() / 2);
}

// Hightlight edge.
NodesEdge HamiltonianLoop::GetHightlightEdge(IndexType index) const
{
    NodesEdge res;
    res.source = m_HamiltonianLoop[2 * index];
    res.target = m_HamiltonianLoop[2 * index + 2];
    res.edgeId = m_HamiltonianLoop[2 * index + 1];
    return res;
}

// Get result count.
IndexType HamiltonianLoop::GetResultCount() const
{
    return (IndexType)(1 + (m_pGraph->IsMultiGraph() ? m_HamiltonianLoop.size() : m_HamiltonianLoop.size() / 2 + 1));
}

// Get result.
AlgorithmResult HamiltonianLoop::GetResult(IndexType index) const
{
    AlgorithmResult result;
    
    if (index == 0)
    {
        result = AlgorithmResult((IntWeightType)m_bResult);
    }
    else if (index < m_HamiltonianLoop.size() + 1)
    {
        if (m_pGraph->IsMultiGraph())
        {
            if (index % 2 == 1)
            {
                result.type = ART_NODES_PATH;
                m_pGraph->GetNodeStrId(m_HamiltonianLoop[index - 1], result.strValue,
                                   sizeof(result.strValue));
            }
            else
            {
                result.type = ART_EDGES_PATH;
                m_pGraph->GetEdgeStrId(m_HamiltonianLoop[index - 1], result.strValue,
                                   sizeof(result.strValue));
            }
        }
        else
        {
            result.type = ART_NODES_PATH;
            m_pGraph->GetNodeStrId(m_HamiltonianLoop[2 * (index - 1)], result.strValue,
                                   sizeof(result.strValue));
            
        }
    }
    
    return result;
}

// Get propery
bool HamiltonianLoop::GetNodeProperty(ObjectId object, IndexType index, AlgorithmResult* param) const
{
    bool res = false;
    if (index == 0 && param)
    {
        auto position = std::find(m_HamiltonianLoop.begin(), m_HamiltonianLoop.end(), object);
        
        if (position != m_HamiltonianLoop.end())
        {
            param->type = ART_INT;
            param->nValue = (IntWeightType)(position - m_HamiltonianLoop.begin());
            res = true;
        }
    }
    return res;
}

const char* HamiltonianLoop::GetNodePropertyName(IndexType index) const
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

bool HamiltonianLoop::_FindHamiltonianLoopRecursive(int currentNodeNumber, const std::vector<std::vector<IndexType>> & reachabilityMatrix, std::vector<int> & path, std::vector<int> & step)
{
    IndexType v  = 0;
    bool q1 = false;
    int nodesCount = reachabilityMatrix.size();
    ObjectId currentNodeIndex = path[currentNodeNumber - 1];
    const auto & targets = reachabilityMatrix.at(currentNodeIndex);
    
    for(v = 0; v < targets.size() && !q1; v++)
    {
        auto nodeIndex = targets.at(v);
        if (currentNodeNumber == nodesCount && nodeIndex == m_startNode)
            q1 = true;
        else if (step[nodeIndex] == -1)
        {
            step[nodeIndex] = currentNodeNumber;
            path[currentNodeNumber] = nodeIndex;

            q1 = _FindHamiltonianLoopRecursive (currentNodeNumber + 1, reachabilityMatrix, path, step);
            if (!q1)
                step[nodeIndex] = -1;
        }
        else
        {
            continue;
        }
    }
    
    return q1;
}

void HamiltonianLoop::FillLoop(const std::vector<int> & path, const IGraph* currentGraph, IndexType nodesCount)
{
    int realNodesCount = m_pGraph->GetNodesCount();
    m_HamiltonianLoop.resize(realNodesCount + (nodesCount > 1 ? 1 + realNodesCount: 0));

    int elementNum = 0;

    if (m_pGraph->IsMultiGraph())
    {
        const IMultiGraph* multiGraph = m_pAlgorithmFactory->CreateMultiGraph(m_pGraph);
        std::unordered_set<ObjectId> checkCount;
        
        for (int index : path)
        {
            if (elementNum > 0)
            {
                int prevNum   = elementNum - 1;
                ObjectId edge = multiGraph->GetEdge(m_HamiltonianLoop[prevNum], multiGraph->GetNode(index), 0);
                if (checkCount.find(edge) != checkCount.end())
                {
                    edge = multiGraph->GetEdge(m_HamiltonianLoop[prevNum], multiGraph->GetNode(index), 1);
                }
                assert(checkCount.find(edge) == checkCount.end());
                checkCount.insert(edge);
                m_HamiltonianLoop[elementNum++] = edge;
            }
            
            m_HamiltonianLoop[elementNum++] = multiGraph->GetNode(index);
        }

        if (nodesCount > 1)
        {
            int prevNum = elementNum - 1;
            
            ObjectId edge = multiGraph->GetEdge(m_HamiltonianLoop[prevNum], m_HamiltonianLoop[0], 0);
            if (checkCount.find(edge) != checkCount.end())
            {
                edge = multiGraph->GetEdge(m_HamiltonianLoop[prevNum], m_HamiltonianLoop[0], 1);
            }
            assert(checkCount.find(edge) == checkCount.end());
            checkCount.insert(edge);
            
            m_HamiltonianLoop[elementNum++]   = edge;
            m_HamiltonianLoop[elementNum] = m_HamiltonianLoop[0];
        }
    }
    else
    {
        for (int index : path)
        {
            if (elementNum > 0)
            {
                int prevNum = elementNum - 1;
                m_HamiltonianLoop[elementNum++] = currentGraph->GetEdge(m_HamiltonianLoop[prevNum], currentGraph->GetNode(index));
            }
            m_HamiltonianLoop[elementNum++] = currentGraph->GetNode(index);
        }

        if (nodesCount > 1)
        {
            int prevNum = elementNum - 1;
            m_HamiltonianLoop[elementNum++] = currentGraph->GetEdge(m_HamiltonianLoop[prevNum], m_HamiltonianLoop[0]);
            m_HamiltonianLoop[elementNum] = m_HamiltonianLoop[0];
        }
    }
    
    m_bResult = (elementNum == (realNodesCount + realNodesCount)) || nodesCount == 1;
}
