//
//  ConnectedСomponent.cpp
//  Graphoffline
//
//  Created by Олег on 27.12.15.
//
//

#include "ConnectedComponent.h"
#include <unordered_set>
#include <queue>
#include <stack>
#include <string.h>

const char* g_strongParam = "strong";

ConnectedComponent::ConnectedComponent ()
{
    m_pGraph = nullptr;
    m_nConnectedCompCount = 0;
    m_bStrongComponent = false;
}

ConnectedComponent::~ConnectedComponent ()
{
    
}

// Enum parameters
bool ConnectedComponent::EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const
{
    if (index == 0)
    {
        strncpy(outParamInfo->paramName, g_strongParam, sizeof(outParamInfo->paramName));
        outParamInfo->type = APT_FLAG;
        return true;
    }
    
    return false;
}

// Set parameter to algorithm.
void ConnectedComponent::SetParameter(const AlgorithmParam* outParamInfo)
{
    if (outParamInfo && strncmp(outParamInfo->paramName, g_strongParam, sizeof(AlgorithmParam().paramName)) == 0)
    {
        m_bStrongComponent = outParamInfo->data.bFlag;
    }
}

// Calculate algorithm.
bool ConnectedComponent::Calculate()
{
    m_nConnectedCompCount = 0;
    bool res = false;
    
    if (m_pGraph->GetNodesCount() > 0)
    {
        // Find strong connected component.
        if (m_bStrongComponent && m_pGraph->HasDirected())
        {
            res = FindStrongComponent();
        }
        else
        {
            res = FindWeakComponent();
        }
    }
    
    return res;
}


// Get result count.
IndexType ConnectedComponent::GetResultCount() const
{
    return 1;
}

// Get result.
AlgorithmResult ConnectedComponent::GetResult(IndexType index) const
{
    return AlgorithmResult(m_nConnectedCompCount);
}

bool ConnectedComponent::FindWeakComponent()
{
    bool res = false;
    // Find weak connected component or connected component.
    GraphPtr pGraph = GraphPtr(m_pGraph->MakeBaseCopy(GTC_MAKE_UNDIRECTED));
    
    if (pGraph)
    {
        std::unordered_set<ObjectId> processedNodes;
        
        while (processedNodes.size() != pGraph->GetNodesCount())
        {
            // Find Next comonent.
            ObjectId firstComponentNode = 0;
            for (int i = 0; i < pGraph->GetNodesCount(); i ++)
            {
                firstComponentNode = pGraph->GetNode(i);
                if (processedNodes.count(firstComponentNode) == 0)
                {
                    processedNodes.insert(firstComponentNode);
                    break;
                }
            }
            
            // Find all nodes from this component.
            std::queue<ObjectId> currentQuary;
            currentQuary.push(firstComponentNode);
            while (!currentQuary.empty())
            {
                ObjectId currentNode = currentQuary.front();
                currentQuary.pop();
                for (int i = 0; i < pGraph->GetConnectedNodes(currentNode); i ++)
                {
                    ObjectId neighborhoodNode = pGraph->GetConnectedNode(currentNode, i);
                    if (processedNodes.count(neighborhoodNode) == 0)
                    {
                        currentQuary.push(neighborhoodNode);
                        processedNodes.insert(neighborhoodNode);
                    }
                }
            }
            
            m_nConnectedCompCount++;
        }
        res = true;
    }
    
    return res;
}

class EnumStrategy : public IEnumStrategy
{
public:
    
    EnumStrategy (bool saveInStack) : m_saveInStack(saveInStack) {;}

    // We started process this node.
    void StartProcessNode(ObjectId nodeId) override
    {
        m_visited.insert(nodeId);
    }
    // @return true if we need to process this child node.
    bool NeedProcessChild(ObjectId nodeId, ObjectId childId, ObjectId edgeId) override
    {
        return m_visited.find(childId) == m_visited.end();
    }
    // We finish process this node.
    void FinishProcessNode(ObjectId nodeId) override
    {
        if (m_saveInStack)
        {
            m_stack.push(nodeId);
        }
    }
    
    // Default Strategy.
    DefaultEnumStrategy GetDefaultStrategy() override
    {
        return IEnumStrategy::DES_NODE;
    }
    
    std::stack<ObjectId>& GetStack ()
    {
        return m_stack;
    }
    
    // We started process this edge.
    void StartProcessEdge(ObjectId edgeId) override {}
    // We finish process this edge.
    void FinishProcessEdge(ObjectId edgeId) override {}
    
protected:
    std::unordered_set<ObjectId> m_visited;
    std::stack<ObjectId>         m_stack;
    bool                         m_saveInStack;
};

bool ConnectedComponent::FindStrongComponent()
{
    bool res = false;
    
    GraphPtr pGraph = GraphPtr(m_pGraph->MakeBaseCopy(GTC_INVERSE));
    EnumStrategy invertseEnum(true);
    
    for (IndexType i = 0; i < pGraph->GetNodesCount(); i++)
    {
        ObjectId node = pGraph->GetNode(i);
        if (invertseEnum.NeedProcessChild(0, node, 0))
        {
            pGraph->ProcessDFS(&invertseEnum, node);
        }
    }
    
    std::stack<ObjectId>&  stack = invertseEnum.GetStack();
    
    EnumStrategy forwardEnum(false);
    
    while (!stack.empty())
    {
        ObjectId node = stack.top();
        stack.pop();
        
        if (forwardEnum.NeedProcessChild(0, node, 0))
        {
            m_pGraph->ProcessDFS(&forwardEnum, node);
            m_nConnectedCompCount++;
        }
    }
    
    return res;
}


