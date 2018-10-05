//
//  EulerianPath.hpp
//  Graphoffline
//
//  Created by Олег on 26.12.15.
//
//

#ifndef EulerianPath_hpp
#define EulerianPath_hpp

#include <stdio.h>
#include "BaseAlgorithm.h"
#include "IGraph.h"
#include <vector>

class EulerianPath : public BaseAlgorithm
{
public:
    EulerianPath (bool loop);
    virtual ~EulerianPath ();
    
    // Long name of algoright: DijkstraShortPath.
    virtual const char* GetFullName() const override {return (m_loop ? "Eulerian Loop" : "Eulerian Path");};
    // Short name of algorithm: dsp
    virtual const char* GetShortName() const override {return (m_loop ? "elloop" : "elpath"); };
    // Calculate algorithm.
    virtual bool Calculate() override ;
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
    
    // Search loop.
    const bool m_loop;
    
    bool _FindEulerianLoopRecursive(GraphPtr pGraph, ObjectId node);
    
    void RemoveFakeFromLoop(GraphPtr graph, ObjectId start, ObjectId finish);
    
    void AddEdgeForPath(GraphPtr graph, ObjectId start, ObjectId finish, bool direct);
    
    // Has or not EulerLoop.
    bool m_bResult;
    std::vector<ObjectId> m_EulerianLoop;
};

#endif /* EulerianLoop_hpp */
