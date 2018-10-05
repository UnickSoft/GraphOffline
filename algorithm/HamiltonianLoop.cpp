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
            
            bCanHasLoop = (componentCount == 1);
        }

        if (bCanHasLoop)
        {
            std::vector<std::vector<bool>> adjacencyMatrix = GetAdjacencyMatrixBool(*currentGraph);
    
            int nodesCount = currentGraph->GetNodesCount();

            std::vector<int> path;
            path.resize(nodesCount);
            std::fill(path.begin(), path.end(), -1);

            std::vector<int> step;
            step.resize(nodesCount);
            std::fill(step.begin(), step.end(), -1);
            
            m_startNode = m_path ? nodesCount - 1 : 0;
            
            path[0] = m_startNode;
            step[m_startNode] = m_startNode;
            
            m_bResult = _FindHamiltonianLoopRecursive(1, adjacencyMatrix, path, step);
            
            if (nodesCount == 1)
            {
                m_bResult = true;
            }
            
            if (m_bResult)
            {
                int realNodesCount = m_pGraph->GetNodesCount();
                
                if (m_path)
                {
                    m_HamiltonianLoop.resize(realNodesCount);
                
                    int elementNum = 0;
                    bool first = true;
                    for (int index : path)
                    {
                        if (first) { first = false; continue; }
                        
                        m_HamiltonianLoop[elementNum++] = currentGraph->GetNode(index);
                    }
                    
                    m_bResult = (elementNum == realNodesCount);
                }
                else // loop
                {
                    int realNodesCount = m_pGraph->GetNodesCount();
                    m_HamiltonianLoop.resize(realNodesCount + (nodesCount > 1 ? 1 : 0));
                    
                    int elementNum = 0;
                    for (int index : path)
                    {
                        m_HamiltonianLoop[elementNum++] = currentGraph->GetNode(index);
                    }
                    
                    if (nodesCount > 0)
                        m_HamiltonianLoop[realNodesCount] = m_HamiltonianLoop[0];
                    
                    m_bResult = (elementNum == realNodesCount);
                }
            }
        }
    }

    return true;
}

// Hightlight nodes count.
IndexType HamiltonianLoop::GetHightlightNodesCount() const
{
    return (IndexType)m_HamiltonianLoop.size();
}

// Hightlight node.
ObjectId HamiltonianLoop::GetHightlightNode(IndexType index) const
{
    return m_HamiltonianLoop[index];
}

// Hightlight edges count.
IndexType HamiltonianLoop::GetHightlightEdgesCount() const
{
    return (IndexType)(m_HamiltonianLoop.size() ? m_HamiltonianLoop.size() - 1 : 0);
}

// Hightlight edge.
NodesEdge HamiltonianLoop::GetHightlightEdge(IndexType index) const
{
    NodesEdge edge;
    edge.source = m_HamiltonianLoop[index];
    edge.target = m_HamiltonianLoop[index + 1];
    return edge;
}


// Get result count.
IndexType HamiltonianLoop::GetResultCount() const
{
    return (IndexType)(1 + m_HamiltonianLoop.size());
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
        result.type = ART_NODES_PATH;
        m_pGraph->GetNodeStrId(m_HamiltonianLoop[index - 1], result.strValue,
                               sizeof(result.strValue));
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

bool HamiltonianLoop::_FindHamiltonianLoopRecursive(int currentNodeNumber, const std::vector<std::vector<bool>> & adjacencyMatrix, std::vector<int> & path, std::vector<int> & step)
{
    int v  = 0;
    bool q1 = false;
    int nodesCount = adjacencyMatrix.size();
    
    for(v = 0; v < nodesCount && !q1; v++)
    {
        if (adjacencyMatrix[path[currentNodeNumber - 1]][v])
        {
            if (currentNodeNumber == nodesCount && v == m_startNode )
                q1 = true;
            else if (step[v] == -1)
            {
                step[v] = currentNodeNumber;
                path[currentNodeNumber] = v;
                
                q1 = _FindHamiltonianLoopRecursive (currentNodeNumber + 1, adjacencyMatrix, path, step);
                if (!q1)
                    step[v] = -1;
            }
            else
                continue;
        }
    }
    
    return q1;
}
