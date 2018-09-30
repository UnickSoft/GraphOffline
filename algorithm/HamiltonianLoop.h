//
//  HamiltonianLoop.hpp
//  Graphoffline
//
//  Created by Олег on 26.12.15.
//
//

#pragma once

#include <stdio.h>
#include "BaseAlgorithm.h"
#include "IGraph.h"
#include <vector>

class HamiltonianLoop : public BaseAlgorithm
{
public:
    HamiltonianLoop (bool loop);
    virtual ~HamiltonianLoop ();
    
    // Long name of algoright: DijkstraShortPath.
    virtual const char* GetFullName() const override {return (m_path ? "Hamiltonian Path" : "Hamiltonian Loop");};
    // Short name of algorithm: dsp
    virtual const char* GetShortName() const override {return (m_path ? "hampath" : "hamloop"); };
    // Calculate algorithm.
    virtual bool Calculate() override;
    // Hightlight nodes count.
    virtual IndexType GetHightlightNodesCount() const override;
    // Hightlight node.
    virtual ObjectId GetHightlightNode(IndexType index) const override;
    // Hightlight edges count.
    virtual IndexType GetHightlightEdgesCount() const override;
    // Hightlight edge.
    virtual NodesEdge GetHightlightEdge(IndexType index) const override;
    // Get result count.
    virtual IndexType GetResultCount() const override;
    // Get result of index. Algorithms can have complex result.
    virtual AlgorithmResult GetResult(IndexType index) const override;
    // Get propery
    virtual bool GetNodeProperty(ObjectId object, IndexType index, AlgorithmResult* param) const override;
    virtual const char* GetNodePropertyName(IndexType index) const override;
    
private:

    bool _FindHamiltonianLoopRecursive(int currentNodeNumber, const std::vector<std::vector<bool>> & adjacencyMatrix, std::vector<int> & path, std::vector<int> & step);
    
    // Search loop.
    const bool m_path;
    
    // Has or not EulerLoop.
    bool m_bResult;
    std::vector<ObjectId> m_HamiltonianLoop;
    
    int m_startNode = 0;
};

