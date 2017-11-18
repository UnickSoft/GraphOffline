//
//  EulerianPath.cpp
//  Graphoffline
//
//  Created by Олег on 26.12.15.
//
//

#include "EulerianPath.h"
#include "IAlgorithmFactory.h"
#include <list>
#include <string.h>
#include <algorithm>

static const char* g_indexStr = "index";
static const char* g_indexCountStr = "indexCount";

EulerianPath::EulerianPath (bool loop) : m_pGraph(NULL), m_bResult(false), m_loop(loop)
{
    
}

EulerianPath::~EulerianPath ()
{
    
}

// Enum parameters
bool EulerianPath::EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const
{
    return false;
}


// Set parameter to algorithm.
void EulerianPath::SetParameter(const AlgorithmParam* outParamInfo)
{
    
}

// Set graph
void EulerianPath::SetGraph(const IGraph* pGraph)
{
    m_pGraph = pGraph;
}

// Calculate algorithm.
bool EulerianPath::EulerianPath::Calculate()
{
    m_bResult = false;
    
    GraphPtr pGraph = GraphPtr(m_pGraph->MakeBaseCopy(GCT_COPY));
    if (!pGraph)
    {
        return false;
    }
    
    AlgorithmPtr connectedComponent = AlgorithmPtr(m_pAlgorithmFactory->CreateAlgorithm("concomp", pGraph.get()));
    
    if (connectedComponent)
    {
        bool bCanHasLoop   = false;
        bool greedyTry     = false; // Try greedy algorithm to find path.
        
        std::vector<ObjectId> odd_vertex;
        
        // Find in full directed graph.
        if (pGraph->HasDirected() && !pGraph->HasUndirected())
        {
            AlgorithmParam param;
            strncpy(param.paramName, "strong", sizeof(param.paramName));
            param.data.bFlag = true;
            
            //Search strong connected component.
            connectedComponent->SetParameter(&param);
            
            connectedComponent->Calculate();
            IntWeightType componentCount = connectedComponent->GetResult(0).nValue;
            
            if ((componentCount == 1 && m_loop) || (componentCount >= 1 && !m_loop))
            {
                bCanHasLoop = true;
                ObjectId startObject  = -1;
                ObjectId finishObject = -1;
                
                // Try to connect 2 nodes to has loop.
                for (IndexType i = 0; i < pGraph->GetNodesCount(); i ++)
                {
                    ObjectId node = pGraph->GetNode(i);
                    // Has odd node.
                    if (pGraph->GetConnectedNodes(node) != pGraph->GetSourceNodesNumber(node))
                    {
                        bCanHasLoop = false;
                    }
                    
                    if (pGraph->GetConnectedNodes(node) + 1 == pGraph->GetSourceNodesNumber(node)&& startObject == -1)
                    {
                        startObject = node;
                    }
                    
                    if (pGraph->GetConnectedNodes(node) - 1 == pGraph->GetSourceNodesNumber(node)&& finishObject == -1)
                    {
                        finishObject = node;
                    }
                }
                
                // If we serach path and has no loop, try to connect.
                if (!m_loop && !bCanHasLoop && startObject != -1 && finishObject != -1)
                {
                    pGraph->AddEdge(startObject, finishObject, false, 1.0);
                    odd_vertex.push_back(startObject);
                    odd_vertex.push_back(finishObject);
                    bCanHasLoop = true;
                }
            }
        } // Find in full undirected graph.
        else if (!pGraph->HasDirected() && pGraph->HasUndirected())
        {
            connectedComponent->Calculate();
            IntWeightType componentCount = connectedComponent->GetResult(0).nValue;
            
            if (componentCount == 1)
            {
                bCanHasLoop = true;
                
                for (IndexType i = 0; i < pGraph->GetNodesCount(); i ++)
                {
                    ObjectId node = pGraph->GetNode(i);
                    // Has odd node.
                    if (pGraph->GetConnectedNodes(node) % 2 == 1)
                    {
                        bCanHasLoop = false;
                        if (!m_loop)
                        {
                            odd_vertex.push_back(node);
                        }
                    }
                }
                
                if (!m_loop && odd_vertex.size() == 2)
                {
                    // Create the same second egde.
                    if (pGraph->IsEgdeExists(odd_vertex[0], odd_vertex[1]))
                    {
                        auto fakeNodeId = pGraph->AddNode(true);
                        pGraph->AddEdge(odd_vertex[0], fakeNodeId, false, 1.0);
                        pGraph->AddEdge(fakeNodeId, odd_vertex[1], false, 1.0);
                    }
                    else
                    {
                        pGraph->AddEdge(odd_vertex[0], odd_vertex[1], false, 1.0);
                    }
                    bCanHasLoop = true;
                }
            }
        }
        else
        {
            AlgorithmParam param;
            strncpy(param.paramName, "strong", sizeof(param.paramName));
            param.data.bFlag = true;
            
            //Search strong connected component.
            connectedComponent->SetParameter(&param);
            
            connectedComponent->Calculate();
            IntWeightType componentCount = connectedComponent->GetResult(0).nValue;
            
            bCanHasLoop = (componentCount == 1);
            greedyTry   = (componentCount <= 2) && !m_loop;
        }
        
        if (bCanHasLoop || greedyTry)
        {
            {
                auto edgesNumber = pGraph->GetEdgesCount();
                
                if (greedyTry)
                {
                    GraphPtr pCopyGraph = GraphPtr(pGraph->MakeBaseCopy(GCT_COPY));
                    m_bResult = _FindEulerianLoopRecursive(pCopyGraph, (odd_vertex.size() == 2) ? odd_vertex[1] : pCopyGraph->GetNode((IndexType)0));
                }
                else
                {
                    m_bResult = _FindEulerianLoopRecursive(pGraph, (odd_vertex.size() == 2) ? odd_vertex[1] : pGraph->GetNode((IndexType)0));
                }
                if (m_bResult)
                {
                    m_bResult = (m_EulerianLoop.size() - 1 == edgesNumber);
                    if (!m_bResult)
                    {
                        m_EulerianLoop.clear();
                        
                        // Try greedy algorithm for path.
                        if (greedyTry)
                        {
                            // Run greedy. Create edge between all pair of all vertexes.
                            // TODO: Optimize greedy, use not all vertexes.
                            for (IndexType i = 0; i < pGraph->GetNodesCount() && m_EulerianLoop.size() == 0; i ++)
                            {
                                for (IndexType j = 0; j < pGraph->GetNodesCount() && m_EulerianLoop.size() == 0; j ++)
                                {
                                    auto start  = pGraph->GetNode(i);
                                    auto finish = pGraph->GetNode(j);
                                    
                                    if (i != j && !pGraph->IsEgdeExists(start, finish, false))
                                    {
                                        GraphPtr pCopyGraph = GraphPtr(pGraph->MakeBaseCopy(GCT_COPY));
                                        pCopyGraph->AddEdge(start, finish, true, 1.0);
                                        auto edgesNumber = pCopyGraph->GetEdgesCount();
                                        m_bResult = _FindEulerianLoopRecursive(pCopyGraph, finish);
                                        if (m_bResult)
                                        {
                                            m_bResult = (m_EulerianLoop.size() - 1 == edgesNumber);
                                            if (!m_bResult)
                                            {
                                                m_EulerianLoop.clear();
                                            }
                                            else
                                            {
                                                m_EulerianLoop.erase(m_EulerianLoop.end() - 1);
                                                for (int k = 0; k < m_EulerianLoop.size() - 1; k++)
                                                {
                                                    if (m_EulerianLoop[k] == start && m_EulerianLoop[k + 1] == finish)
                                                    {
                                                        m_EulerianLoop.insert(m_EulerianLoop.end(), m_EulerianLoop.begin(), m_EulerianLoop.begin() + k + 1);
                                                        m_EulerianLoop.erase(m_EulerianLoop.begin(), m_EulerianLoop.begin() + k + 1);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (odd_vertex.size() == 2)
                    {
                        m_EulerianLoop.erase(m_EulerianLoop.end() - 1);
                        m_EulerianLoop.erase(std::remove_if(m_EulerianLoop.begin(), m_EulerianLoop.end(), [pGraph](const ObjectId& idObj){
                            return pGraph->IsFakeNode(idObj);
                        }), m_EulerianLoop.end());
                    }
                }
            }
        }
    }
    
    return true;
}

// Hightlight nodes count.
IndexType EulerianPath::GetHightlightNodesCount() const
{
    return m_EulerianLoop.size();
}

// Hightlight node.
ObjectId EulerianPath::GetHightlightNode(IndexType index) const
{
    return m_EulerianLoop[index];
}

// Hightlight edges count.
IndexType EulerianPath::GetHightlightEdgesCount() const
{
    return m_EulerianLoop.size() ? m_EulerianLoop.size() - 1 : 0;
}

// Hightlight edge.
NodesEdge EulerianPath::GetHightlightEdge(IndexType index) const
{
    NodesEdge edge;
    edge.source = m_EulerianLoop[index];
    edge.target = m_EulerianLoop[index + 1];
    return edge;
}


// Get result count.
IndexType EulerianPath::GetResultCount() const
{
    return 1 + m_EulerianLoop.size();
}

// Get result.
AlgorithmResult EulerianPath::GetResult(IndexType index) const
{
    AlgorithmResult result;
    
    if (index == 0)
    {
        result = AlgorithmResult((IntWeightType)m_bResult);
    }
    else if (index < m_EulerianLoop.size() + 1)
    {
        result.type = ART_NODES_PATH;
        m_pGraph->GetNodeStrId(m_EulerianLoop[index - 1], result.strValue,
                               sizeof(result.strValue));
    }
    
    return result;
}

// Get propery
bool EulerianPath::GetProperty(ObjectId object, IndexType index, AlgorithmResult* param) const
{
    bool res = false;
    if (index == 0 && param)
    {
        auto position = std::find(m_EulerianLoop.begin(), m_EulerianLoop.end(), object);
        
        if (position != m_EulerianLoop.end())
        {
            param->type = ART_INT;
            param->nValue = (IntWeightType)(position - m_EulerianLoop.begin());
            res = true;
        }
    }
    return res;
}

const char* EulerianPath::GetPropertyName(IndexType index) const
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

void EulerianPath::SetAlgorithmFactory(const IAlgorithmFactory* pAlgorithmFactory)
{
    m_pAlgorithmFactory = pAlgorithmFactory;
}

class FindLoopStrategy : public IEnumStrategy
{
public:
    
    FindLoopStrategy (ObjectId startNode) : m_startNode(startNode), m_bLoopFound(false) {;}
    
    // We started process this node.
    void StartProcessNode(ObjectId nodeId)
    {
        if (!m_bLoopFound)
        {
            m_loop.push_back(nodeId);
            // Find end of loop.
            if (m_loop.size() > 1 && nodeId == m_startNode)
            {
                m_bLoopFound = true;
            }
        }
    }
    // @return true if we need to process this child node.
    bool NeedProcessChild(ObjectId nodeId, ObjectId childId, ObjectId edge)
    {
        return !m_bLoopFound;
    }
    // We finish process this node.
    void FinishProcessNode(ObjectId nodeId)
    {
        if (!m_bLoopFound)
        {
            m_loop.pop_back();
        }
    }
    
    // Default Strategy.
    virtual DefaultEnumStrategy GetDefaultStrategy()
    {
        return IEnumStrategy::DES_EDGE;
    }
    
    const std::list<ObjectId>& GetLoop()
    {
        return m_loop;
    }
    
    bool HasLoop()
    {
        return m_bLoopFound;
    }
    
protected:
    bool m_bLoopFound;
    ObjectId m_startNode;
    std::list<ObjectId> m_loop;
};

bool EulerianPath::_FindEulerianLoopRecursive(GraphPtr pGraph, ObjectId node)
{
    bool res = false;
    
    FindLoopStrategy findLoop(node);
    
    pGraph->ProcessDFS(&findLoop, node);
    
    if (findLoop.HasLoop())
    {
        res = true;
        std::list<ObjectId> loop = findLoop.GetLoop();
        
        auto start = loop.begin();
        start++;
        for (auto iterator = start; iterator != loop.end(); iterator++)
        {
            auto prevNode = iterator;
            prevNode--;
            pGraph->RemoveEdge(*prevNode, *iterator);
        }
        
        while (!loop.empty())
        {
            ObjectId node = loop.front();
            loop.pop_front();
            bool bFound = _FindEulerianLoopRecursive(pGraph, node);
            // It will be already added early.
            if (!bFound)
            {
                m_EulerianLoop.push_back(node);
            }
        }
    }
    
    return res;
}



