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
    
    GraphPtr pGraph = GraphPtr(m_pGraph->MakeBaseCopy(GTC_REMOVE_SELF_LOOP));
    if (!pGraph)
    {
        return false;
    }
    
    AlgorithmPtr connectedComponent = AlgorithmPtr(m_pAlgorithmFactory->CreateAlgorithm("concomp", pGraph.get()));
    
    if (connectedComponent)
    {
        bool bCanHas = false;
        
        if (pGraph->IsDirected())
        {
            
        }
        else
        {
            connectedComponent->Calculate();
            IntWeightType componentCount = connectedComponent->GetResult().nValue;
            
            if (componentCount == 1)
            {
                bCanHas = true;
                for (IndexType i = 0; (i < pGraph->GetNodesCount()) && bCanHas; i ++)
                {
                    ObjectId node = pGraph->GetNode(i);
                    // Has odd node.
                    if (pGraph->GetConnectedNodes(i) % 2 == 1)
                    {
                        bCanHas = false;
                    }
                }
            }
        }
        
        if (bCanHas)
        {
            m_bResult = _FindEulerianLoopRecursive(pGraph, pGraph->GetNode((IndexType)0));
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

// Get result.
AlgorithmResult EulerianLoop::GetResult() const
{
    return AlgorithmResult((IntWeightType)m_bResult);
}

// Get propery
bool EulerianLoop::GetProperty(ObjectId object, IndexType index, AlgorithmResult* param) const
{
    return false;
}

const char* EulerianLoop::GetPropertyName(IndexType index) const
{
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



