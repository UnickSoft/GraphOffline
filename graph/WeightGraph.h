//
//  WeightGraph.h
//  Graphoffline
//
//  Created by Олег on 26.12.2019.
//

#pragma once

#include <stdio.h>
#include "IGraph.h"
#include "Graph.h"

template<class WeightInterface, typename WeightTmplType> class WeightGraph : public WeightInterface
{
public:
    // Get Egde weight of int graph.
    virtual WeightTmplType GetEdgeWeight(ObjectId source, ObjectId target) const override
    {
        return m_graph.GetEdgeWeight<WeightTmplType>(source, target);
    }
    // Create copy of graph.
    virtual WeightInterface* MakeCopy(GraphCopyType type) const override
    {
        return (WeightInterface*)MakeBaseCopy(type);
    }
  
    // Graph methods
    IndexType GetNodesCount() const override            { return m_graph.GetNodesCount(); }
    IndexType GetEdgesCount() const override            { return m_graph.GetEdgesCount(); }
    ObjectId GetNode(IndexType index) const override    { return m_graph.GetNode(index); }
    ObjectId GetNode(const char* nodeId) const override { return m_graph.GetNode(nodeId); }
    IndexType GetConnectedNodes(ObjectId source) const override { return m_graph.GetConnectedNodes(source); }
    ObjectId GetConnectedNode(ObjectId source, IndexType index) const override { return m_graph.GetConnectedNode(source, index); }
    bool AreNodesConnected(ObjectId source, ObjectId target) const override { return m_graph.AreNodesConnected(source, target); }
    bool GetNodeStrId(ObjectId node, char* outBuffer, IndexType bufferSize) const override { return m_graph.GetNodeStrId( node, outBuffer, bufferSize); }
    bool IsEgdeExists(ObjectId source, ObjectId target, bool onlyInSourceGraph = true) const override { return m_graph.IsEgdeExists( source, target, onlyInSourceGraph); }
    EdgeWeightType GetEdgeWeightType() const override { return m_graph.GetEdgeWeightType(); }
    IGraph* MakeBaseCopy(GraphCopyType type) const override { return m_graph.MakeBaseCopy(type); }
    bool HasDirected() const override { return m_graph.HasDirected(); }
    bool HasUndirected() const override { return m_graph.HasUndirected(); }
    bool IsMultiGraph() const override { return m_graph.IsMultiGraph(); }
    void ProcessDFS(IEnumStrategy* pEnumStrategy, ObjectId startedNode) const override { return m_graph.ProcessDFS(pEnumStrategy, startedNode); }
    void RemoveEdge(ObjectId source, ObjectId target) override { return m_graph.RemoveEdge(source, target); }
    IndexType GetSourceNodesNumber(ObjectId source) override { return m_graph.GetSourceNodesNumber(source); }
    bool AddEdge(ObjectId source, ObjectId target, bool direct, const FloatWeightType& weight) override { return m_graph.AddEdge(source, target, direct, weight); }
    ObjectId AddNode(bool fake) override { return m_graph.AddNode(fake); }
    bool IsFakeNode(ObjectId source) override { return m_graph.IsFakeNode(source); }
    const char* PrintGraph() override  { return m_graph.PrintGraph(); }
    bool LoadFromGraphML(const char * pBuffer, uint32_t bufferSize) { return m_graph.LoadFromGraphML(pBuffer, bufferSize); }
    
protected:

    Graph m_graph;
};

using IntGraph   = WeightGraph<IGraphInt,   IntWeightType>;
using FloatGraph = WeightGraph<IGraphFloat, FloatWeightType>;
