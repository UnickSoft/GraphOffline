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


ConnectedComponent::ConnectedComponent ()
{
    m_pGraph = nullptr;
    m_nConnectedCompCount = 0;
}

ConnectedComponent::~ConnectedComponent ()
{
    
}

// Enum parameters
bool ConnectedComponent::EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const
{
    return 0;
}

// Set parameter to algorithm.
void ConnectedComponent::SetParameter(const char* name, ObjectId id)
{;}

// Set graph
void ConnectedComponent::SetGraph(const IGraph* pGraph)
{
    m_pGraph = pGraph;
}

// Calculate algorithm.
bool ConnectedComponent::Calculate()
{
    m_nConnectedCompCount = 0;
    
    if (m_pGraph->GetNodesCount() > 0)
    {
        std::unordered_set<ObjectId> processedNodes;
        
        while (processedNodes.size() != m_pGraph->GetNodesCount())
        {
            // Find Next comonent.
            ObjectId firstComponentNode = 0;
            for (int i = 0; i < m_pGraph->GetNodesCount(); i ++)
            {
                firstComponentNode = m_pGraph->GetNode(i);
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
                for (int i = 0; i < m_pGraph->GetConnectedNodes(currentNode); i ++)
                {
                    ObjectId neighborhoodNode = m_pGraph->GetConnectedNode(currentNode, i);
                    if (processedNodes.count(neighborhoodNode) == 0)
                    {
                        currentQuary.push(neighborhoodNode);
                        processedNodes.insert(neighborhoodNode);
                    }
                }
            }
            
            m_nConnectedCompCount++;
        }
        
    }
    
    return true;
}

// Hightlight nodes count.
IndexType ConnectedComponent::GetHightlightNodesCount() const
{
    return 0;
}

// Hightlight node.
ObjectId ConnectedComponent::GetHightlightNode(IndexType index) const
{
    return 0;
}

// Hightlight edges count.
IndexType ConnectedComponent::GetHightlightEdgesCount() const
{
    return 0;
}

// Hightlight edge.
NodesEdge ConnectedComponent::GetHightlightEdge(IndexType index) const
{
    return NodesEdge();
}

// Get result.
AlgorithmResult ConnectedComponent::GetResult() const
{
    return AlgorithmResult(m_nConnectedCompCount);
}

// Get propery
bool ConnectedComponent::GetProperty(ObjectId object, IndexType index, AlgorithmResult* param) const  const
{
    return false;
}

const char* ConnectedComponent::GetPropertyName(IndexType index) const
{
    return nullptr;
}

