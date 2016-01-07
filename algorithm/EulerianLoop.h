//
//  EulerianLoop.hpp
//  Graphoffline
//
//  Created by Олег on 26.12.15.
//
//

#ifndef EulerianLoop_hpp
#define EulerianLoop_hpp

#include <stdio.h>
#include "IAlgorithm.h"
#include "IGraph.h"
#include <vector>

class EulerianLoop : public IAlgorithm
{
public:
    EulerianLoop ();
    virtual ~EulerianLoop ();
    
    // Long name of algoright: DijkstraShortPath.
    virtual const char* GetFullName() const {return "Eulerian Loop";};
    // Short name of algorithm: dsp
    virtual const char* GetShortName() const {return "elloop";};
    // Enum parameters
    virtual bool EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const;
    // Set parameter to algorithm.
    virtual void SetParameter(const AlgorithmParam* outParamInfo);
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
    virtual AlgorithmResult GetResult() const;
    // Get propery
    virtual bool GetProperty(ObjectId object, IndexType index, AlgorithmResult* param) const;
    virtual const char* GetPropertyName(IndexType index) const;
    virtual void SetAlgorithmFactory(const IAlgorithmFactory* pAlgorithmFactory);
    
private:
    
    const IGraph* m_pGraph;
    const IAlgorithmFactory* m_pAlgorithmFactory;
    
    bool _FindEulerianLoopRecursive(GraphPtr pGraph, ObjectId node);
    
    // Has or not EulerLoop.
    bool m_bResult;
    std::vector<ObjectId> m_EulerianLoop;
};

#endif /* EulerianLoop_hpp */
