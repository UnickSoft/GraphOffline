/**
 *
 *  Interface for algorithm.
 */


#pragma once

#include "IAlgorithm.h"
#include "IGraph.h"
#include <vector>
#include <map>
#include <unordered_map>

template<class WeightTypeInterface, typename WeightType> class MaxFlowPushRelabel : public BaseAlgorithm
{
public:
    MaxFlowPushRelabel ();
    virtual ~MaxFlowPushRelabel () = default;
    
    // Long name of algoright: DijkstraShortPath.
    virtual const char* GetFullName() const override {return "MaxFlowPushRelable";};
    // Short name of algorithm: dsp
    virtual const char* GetShortName() const override {return "mfpr";};
    // Enum parameters.
    virtual bool EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const override;
    // Set parameter to algorithm.
    virtual void SetParameter(const AlgorithmParam* outParamInfo) override;
    // Set graph.
    virtual void SetGraph(const IGraph* pGraph) override;
    // Calculate algorithm.
    virtual bool Calculate() override;
    // Hightlight edges count.
    virtual IndexType GetHightlightEdgesCount() const override;
    // Hightlight edge.
    virtual NodesEdge GetHightlightEdge(IndexType index) const override;
    // Get result count.
    virtual IndexType GetResultCount() const override;
    // Get result of index. Algorithms can have complex result.
    virtual AlgorithmResult GetResult(IndexType index) const override;
    // Get property.
    virtual bool GetEdgeProperty(const NodesEdge& object, IndexType index, AlgorithmResult* param) const override;
    virtual const char* GetEdgePropertyName(IndexType index) const override;
    
protected:

    void findFlowValues(IndexType drainIndex, IndexType sourceIndex, std::vector<std::vector<WeightType>>& origin_adjacencyMatrix, std::vector<std::vector<WeightType>>& adjacencyMatrix);
    void debugPring();

    ObjectId _source;
    ObjectId _drain;
    const WeightTypeInterface*  _pGraph;
    
    struct EdgeFlowValue
    {
        NodesEdge edge;
        bool backToFront = false;
        WeightType value = (WeightType)0;
    };
    
    std::vector<EdgeFlowValue> _flowValue;
    
    WeightType m_result;
};

#include "MaxFlowPushRelableImpl.h"
