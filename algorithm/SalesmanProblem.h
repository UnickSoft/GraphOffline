//
//  SalesmanProblem.hpp
//  Graphoffline
//
//  Created by Олег on 07.10.25.
//
//

#pragma once

#include <stdio.h>
#include "BaseAlgorithm.h"
#include "IGraph.h"
#include <vector>

class SalesmanProblem : public BaseAlgorithm
{
public:
    SalesmanProblem (bool isFloat, bool isPath);
    virtual ~SalesmanProblem ();

    // Long name of algoright: DijkstraShortPath.
    virtual const char* GetFullName() const override {return m_isPath ? "SalesmanProblem Path" : "SalesmanProblem Loop";};
    // Short name of algorithm: dsp
    virtual const char* GetShortName() const override {return m_isPath ? "slsmenpath" : "slsmen"; };
    // Is support multi graph
    virtual bool IsSupportMultiGraph() const override { return false; }
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

    bool EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const override;
    void SetParameter(const AlgorithmParam* param) override;
    
    // Get propery
    virtual bool GetNodeProperty(ObjectId object, IndexType index, AlgorithmResult* param) const override;
    virtual const char* GetNodePropertyName(IndexType index) const override;

    virtual void UnitTest() const override {}

private:

    bool m_isFloat = false;
    bool m_bResult = false;
    bool m_isPath = false;
    ObjectId m_startNode = 0;
    std::vector<ObjectId> m_path;

    int32_t  m_minCostInt = 0;
    float    m_minCostFloat = 0;

};
