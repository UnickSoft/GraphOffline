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

class DijkstraShortPath: public IAlgorithm
{
public:
    DijkstraShortPath ();
    virtual ~DijkstraShortPath ();
    
    // Long name of algoright: DijkstraShortPath.
    virtual const char* GetFullName() const {return "DijkstraShortPath";};
    // Short name of algorithm: dsp
    virtual const char* GetShortName() const {return "dsp";};
    // Enum parameters
    virtual bool EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const;
    // Set parameter to algorithm.
    virtual void SetParameter(const char* name, ObjectId id);
    // Set graph
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
    virtual IntWeightType GetResult() const;
    // Get propery
    virtual IntWeightType GetProperty(ObjectId, const char* name) const;
    
protected:
    ObjectId m_source;
    ObjectId m_target;
    const IGraph*  m_pGraph;
    std::vector<ObjectId> m_path;
    std::unordered_map<ObjectId, int> m_lowestDistance;
    
    int m_result;
};
