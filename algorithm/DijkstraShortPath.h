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

template<class WeightTypeInterface, typename WeightType> class DijkstraShortPath : public IAlgorithm
{
public:
    DijkstraShortPath ();
    virtual ~DijkstraShortPath ();
    
    // Long name of algoright: DijkstraShortPath.
    virtual const char* GetFullName() const {return "DijkstraShortPath";};
    // Short name of algorithm: dsp
    virtual const char* GetShortName() const {return "dsp";};
    // Enum parameters.
    virtual bool EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const;
    // Set parameter to algorithm.
    virtual void SetParameter(const AlgorithmParam* outParamInfo);
    // Set graph.
    virtual void SetGraph(const IGraph* pGraph);
    // Calculate algorithm.
    virtual bool Calculate();
    // Hightlight nodes count.
    virtual IndexType GetHightlightNodesCount() const;
    // Hightlight node.
    virtual ObjectId GetHightlightNode(IndexType index) const;
    // Hightlight edges count.
    virtual IndexType GetHightlightEdgesCount() const;
    // Hightlight edge.
    virtual NodesEdge GetHightlightEdge(IndexType index) const;
    // Get result.
    virtual AlgorithmResult GetResult() const;
    // Get property.
    virtual bool GetProperty(ObjectId object, IndexType index, AlgorithmResult* param) const;
    virtual const char* GetPropertyName(IndexType index) const;
    virtual void SetAlgorithmFactory(const IAlgorithmFactory* pAlgorithmFactory);
    
protected:
    ObjectId m_source;
    ObjectId m_target;
    const WeightTypeInterface*  m_pGraph;
    std::vector<ObjectId> m_path;
    std::unordered_map<ObjectId, WeightType> m_lowestDistance;
    
    WeightType m_result;
};

#include "DijkstraShortPathImpl.h"
