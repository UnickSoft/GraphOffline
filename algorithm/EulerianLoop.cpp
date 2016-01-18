//
//  EulerianLoop.cpp
//  Graphoffline
//
//  Created by Олег on 26.12.15.
//
//

#include "EulerianLoop.h"
#include "IAlgorithmFactory.h"
#include <list>
#include <string.h>
#include <algorithm>

static const char* g_indexStr = "index";
static const char* g_indexCountStr = "indexCount";

EulerianLoop::EulerianLoop () : m_pGraph(NULL), m_bResult(false)
{
    
}

EulerianLoop::~EulerianLoop ()
{
    
}

// Enum parameters
bool EulerianLoop::EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const
{
    return false;
}


// Set parameter to algorithm.
void EulerianLoop::SetParameter(const AlgorithmParam* outParamInfo)
{
    
}

// Set graph
void EulerianLoop::SetGraph(const IGraph* pGraph)
{
    m_pGraph = pGraph;
}

// Calculate algorithm.
bool EulerianLoop::EulerianLoop::Calculate()
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
        bool bCanHas = false;
        
        if (pGraph->HasDirected() && !pGraph->HasUndirected())
        {
            AlgorithmParam param;
            strncpy(param.paramName, "strong", sizeof(param.paramName));
            param.data.bFlag = true;
            
            //Search strong connected component.
            connectedComponent->SetParameter(&param);
            
            connectedComponent->Calculate();
            IntWeightType componentCount = connectedComponent->GetResult(0).nValue;
            
            if (componentCount == 1)
            {
                bCanHas = true;
                
                for (IndexType i = 0; (i < pGraph->GetNodesCount()) && bCanHas; i ++)
                {
                    ObjectId node = pGraph->GetNode(i);
                    // Has odd node.
                    if (pGraph->GetConnectedNodes(node) != pGraph->GetSourceNodesNumber(node))
                    {
                        bCanHas = false;
                    }
                }
            }
        }
        else if (!pGraph->HasDirected() && pGraph->HasUndirected())
        {
            connectedComponent->Calculate();
            IntWeightType componentCount = connectedComponent->GetResult(0).nValue;
            
            if (componentCount == 1)
            {
                bCanHas = true;
                for (IndexType i = 0; (i < pGraph->GetNodesCount()) && bCanHas; i ++)
                {
                    ObjectId node = pGraph->GetNode(i);
                    // Has odd node.
                    if (pGraph->GetConnectedNodes(node) % 2 == 1)
                    {
                        bCanHas = false;
                    }
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
            
            bCanHas = (componentCount == 1);
        }
        
        if (bCanHas)
        {
            m_bResult = _FindEulerianLoopRecursive(pGraph, pGraph->GetNode((IndexType)0));
            if (m_bResult)
            {
                m_bResult = (m_EulerianLoop.size() - 1 == m_pGraph->GetEdgesCount());
                if (!m_bResult)
                {
                    m_EulerianLoop.clear();
                }
            }
        }
    }
    
    return true;
}

// Hightlight nodes count.
IndexType EulerianLoop::GetHightlightNodesCount() const
{
    return m_EulerianLoop.size();
}

// Hightlight node.
ObjectId EulerianLoop::GetHightlightNode(IndexType index) const
{
    return m_EulerianLoop[index];
}

// Hightlight edges count.
IndexType EulerianLoop::GetHightlightEdgesCount() const
{
    return m_EulerianLoop.size() ? m_EulerianLoop.size() - 1 : 0;
}

// Hightlight edge.
NodesEdge EulerianLoop::GetHightlightEdge(IndexType index) const
{
    NodesEdge edge;
    edge.source = m_EulerianLoop[index];
    edge.target = m_EulerianLoop[index + 1];
    return edge;
}


// Get result count.
IndexType EulerianLoop::GetResultCount() const
{
    return 1 + m_EulerianLoop.size();
}

// Get result.
AlgorithmResult EulerianLoop::GetResult(IndexType index) const
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
bool EulerianLoop::GetProperty(ObjectId object, IndexType index, AlgorithmResult* param) const
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

const char* EulerianLoop::GetPropertyName(IndexType index) const
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

void EulerianLoop::SetAlgorithmFactory(const IAlgorithmFactory* pAlgorithmFactory)
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

bool EulerianLoop::_FindEulerianLoopRecursive(GraphPtr pGraph, ObjectId node)
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



