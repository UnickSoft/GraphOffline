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

template<class WeightTypeInterface, typename WeightType> class DijkstraShortPath : public BaseAlgorithm
{
public:
    DijkstraShortPath ();
    virtual ~DijkstraShortPath ();
    
    // Long name of algoright: DijkstraShortPath.
    virtual const char* GetFullName() const override {return "DijkstraShortPath";}
    // Short name of algorithm: dsp
    virtual const char* GetShortName() const override {return "dsp";}
    // Enum parameters.
    virtual bool EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const override;
    // Set parameter to algorithm.
    virtual void SetParameter(const AlgorithmParam* outParamInfo) override;
    // Set graph.
    virtual void SetGraph(const IGraph* pGraph) override;
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
    // Get property.
    virtual bool GetNodeProperty(ObjectId object, IndexType index, AlgorithmResult* param) const override;
    virtual const char* GetNodePropertyName(IndexType index) const override;
    
protected:
    ObjectId m_source;
    ObjectId m_target;
    const WeightTypeInterface*  m_pGraph;
    std::vector<ObjectId> m_path;
    std::unordered_map<ObjectId, WeightType> m_lowestDistance;
    
    WeightType m_result;
};

#include "DijkstraShortPathImpl.h"
