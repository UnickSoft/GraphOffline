//
//  EulerianLoop.cpp
//  Graphoffline
//
//  Created by Олег on 26.12.15.
//
//

#include "EulerianLoop.h"
#include "IAlgorithmFactory.h"

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
    
    AlgorithmPtr connectedComponent = AlgorithmPtr(m_pAlgorithmFactory->CreateAlgorithm("concomp", m_pGraph));
    
    if (connectedComponent)
    {
        if (m_pGraph->IsDirected())
        {
            
        }
        else
        {
            connectedComponent->Calculate();
            IntWeightType componentCount = connectedComponent->GetResult().nValue;
            
            if (componentCount == 1)
            {
                //m_bResult = ;
            }
        }
    }
    
    return true;
}

// Hightlight nodes count.
IndexType EulerianLoop::GetHightlightNodesCount() const
{
    return 0;
}

// Hightlight node.
ObjectId EulerianLoop::GetHightlightNode(IndexType index) const
{
    return 0;
}

// Hightlight edges count.
IndexType EulerianLoop::GetHightlightEdgesCount() const
{
    return 0;
}

// Hightlight edge.
NodesEdge EulerianLoop::GetHightlightEdge(IndexType index) const
{
    return NodesEdge();
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
