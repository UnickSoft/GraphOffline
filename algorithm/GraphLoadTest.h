//
//  GraphSaveLoadTest.hpp
//  Graphoffline
//
//  Created by Олег on 06.01.2020.
//

#pragma once

#include <stdio.h>

#include "BaseAlgorithm.h"
#include "IGraph.h"

class GraphLoadTest : public BaseAlgorithm
{
public:
    GraphLoadTest ();
    virtual ~GraphLoadTest ();

    // Long name of algoright: DijkstraShortPath.
    virtual const char* GetFullName() const  override {return "Graph Save/Load Test";};
    // Short name of algorithm: dsp
    virtual const char* GetShortName() const  override {return "loadtest";};
    // Calculate algorithm.
    virtual bool Calculate() override;
    // Get result count.
    virtual IndexType GetResultCount() const override;
    // Get result of index. Algorithms can have complex result.
    virtual AlgorithmResult GetResult(IndexType index) const override;
    // Hightlight nodes count.
    virtual IndexType GetHightlightNodesCount() const override;
    // Hightlight node.
    virtual ObjectId GetHightlightNode(IndexType index) const override;
    // Hightlight edges count.
    virtual IndexType GetHightlightEdgesCount() const override;
    // Hightlight edge.
    virtual NodesEdge GetHightlightEdge(IndexType index) const override;
    // Support multi graph
    bool IsSupportMultiGraph() const override;
    // Set graph
    void SetGraph(const IGraph* pGraph) override;

    virtual void UnitTest() const override {}

protected:

    std::shared_ptr<IMultiGraph> CreateMultiGraph(const IGraph* pGraph);

    std::shared_ptr<IMultiGraph> m_multiGraph;

};
