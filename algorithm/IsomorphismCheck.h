//
//  ConnectedСomponent.hpp
//  Graphoffline
//
//  Created by Олег on 27.12.15.
//
//

#ifndef Isomorphism_Check_hpp
#define Isomorphism_Check_hpp

#include <stdio.h>

#include <unordered_set>
#include <unordered_map>
#include "BaseAlgorithm.h"
#include "IGraph.h"

class IsomorphismCheck : public BaseAlgorithm
{
public:
    IsomorphismCheck();
    virtual ~IsomorphismCheck();

    // Long name of algoright: DijkstraShortPath.
    virtual const char* GetFullName() const  override {return "Isomorphism Check";};
    // Short name of algorithm: dsp
    virtual const char* GetShortName() const  override {return "isocheck";};
    // Enum parameters
    virtual bool EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const override;
    // Set parameter to algorithm.
    virtual void SetParameter(const AlgorithmParam* outParamInfo) override;
    // Calculate algorithm.
    virtual bool Calculate() override;
    // Get result count.
    virtual IndexType GetResultCount() const override;
    // Get result of index. Algorithms can have complex result.
    virtual AlgorithmResult GetResult(IndexType index) const override;

    bool GetEdgeProperty(const NodesEdge& object, IndexType properyIndex,
      IndexType resultEdgeIndex, AlgorithmResult* param) const override;
    const char* GetEdgePropertyName(IndexType index) const override;

    // Hightlight edges count.
    IndexType GetHightlightEdgesCount() const override;
    // Hightlight edge.
    NodesEdge GetHightlightEdge(IndexType index) const override;

    virtual void UnitTest() const override {}

protected:

    GraphPtr GetSubGraph(const std::unordered_set<ObjectId> & nodesSet);
    void GetGraphAsPairOfNodes(GraphPtr graph, std::vector<int> & result);

    std::unordered_set<ObjectId> m_firstGraphEdges;
    std::unordered_set<ObjectId> m_secondGraphEdges;

    bool                  m_isIsomorph;

    bool                                        m_searchSubgraphs;
    std::vector <std::vector<ObjectId>> m_subGraphs;
    std::unordered_set<ObjectId>        m_edgesInSubGraph;
};

#endif /* Isomorphism_Check_hpp */
