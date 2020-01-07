//
//  GraphSaveLoadTest.cpp
//  Graphoffline
//
//  Created by Олег on 06.01.2020.
//

#include "GraphLoadTest.h"
#include "IAlgorithmFactory.h"

GraphLoadTest::GraphLoadTest ()
{
}

GraphLoadTest::~GraphLoadTest ()
{
}

bool GraphLoadTest::Calculate()
{
    return true;
}

IndexType GraphLoadTest::GetResultCount() const
{
    return 3;
}

AlgorithmResult GraphLoadTest::GetResult(IndexType index) const
{
    switch (index)
    {
        case 0: return AlgorithmResult(m_multiGraph->HasDirected() ? 1 : 0);
        case 1: return AlgorithmResult(m_multiGraph->HasUndirected() ? 1 : 0);
        case 2: return AlgorithmResult(m_multiGraph->IsMultiGraph() ? 1 : 0);
    }
    
    return AlgorithmResult(-1);
}

IndexType GraphLoadTest::GetHightlightNodesCount() const
{
    return m_multiGraph->GetNodesCount();
}

ObjectId GraphLoadTest::GetHightlightNode(IndexType index) const
{
    return m_multiGraph->GetNode(index);
}

IndexType GraphLoadTest::GetHightlightEdgesCount() const
{
    if (m_multiGraph->GetNodesCount() > 1)
    {
      return m_multiGraph->GetEdgesNumber(m_multiGraph->GetNode((IndexType)0), m_multiGraph->GetNode((IndexType)1));
    }
    
    return 0;
}

NodesEdge GraphLoadTest::GetHightlightEdge(IndexType index) const
{
    if (m_multiGraph->GetNodesCount() > 1)
    {
      NodesEdge nodesEdge;
      nodesEdge.source  = m_multiGraph->GetNode((IndexType)0);
      nodesEdge.target  = m_multiGraph->GetNode((IndexType)1);
      nodesEdge.edgeId  = m_multiGraph->GetEdge(nodesEdge.source, nodesEdge.target, index);
      
      return nodesEdge;
    }
    
    return NodesEdge();
}

// Support multi graph
bool GraphLoadTest::IsSupportMultiGraph() const
{
    return true;
}

void GraphLoadTest::SetGraph(const IGraph* pGraph)
{
    m_multiGraph = std::shared_ptr<IMultiGraph>(m_pAlgorithmFactory->CreateMultiGraph(pGraph));
}
