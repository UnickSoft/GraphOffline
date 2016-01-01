//
//  ConnectedСomponent.cpp
//  Graphoffline
//
//  Created by Олег on 27.12.15.
//
//

#include "ConnectedComponent.h"


ConnectedComponent::ConnectedComponent ()
{
    m_pGraph = nullptr;
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
    return false;
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
    return AlgorithmResult();
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

