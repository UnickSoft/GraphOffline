//
//  ConnectedСomponent.hpp
//  Graphoffline
//
//  Created by Олег on 27.12.15.
//
//

#ifndef Connected_Component_hpp
#define Connected_Component_hpp

#include <stdio.h>

#include <stdio.h>
#include "IAlgorithm.h"
#include "IGraph.h"

class ConnectedComponent : public IAlgorithm
{
public:
    ConnectedComponent ();
    virtual ~ConnectedComponent ();
    
    // Long name of algoright: DijkstraShortPath.
    virtual const char* GetFullName() const {return "Connected component";};
    // Short name of algorithm: dsp
    virtual const char* GetShortName() const {return "concomp";};
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
    
protected:
    
    bool FindWeakComponent();
    bool FindStrongComponent();
    
    const IGraph* m_pGraph;
    IntWeightType m_nConnectedCompCount;
    bool m_bStrongComponent;
};

#endif /* Connected_Component_hpp */
