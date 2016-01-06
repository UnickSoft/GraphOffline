/**
 *  Interface for graph.
 *
 */

#pragma once

#include <cstdint>
#include <memory>

// Index value
typedef uint32_t IndexType;
// Id of any object
typedef IndexType ObjectId;
// Integer edge weight
typedef int32_t IntWeightType;
// Float edge weight
typedef double FloatWeightType;

// Type of edges wieght.
enum EdgeWeightType {WT_INT = 0, WT_FLOAT};

// Graph copy type.
enum GraphCopyType {GCT_COPY = 0, GTC_MAKE_UNDIRECTED, GTC_INVERSE};


/**
 * Call back for enum nodes methods (DFS, BSF).
 *
 */
class IEnumStrategy
{
public:
    
    // We started process this node.
    virtual void StartProcessNode(ObjectId nodeId) = 0;
    // @return true if we need to process this child node.
    virtual bool NeedProcessChild(ObjectId nodeId, ObjectId childId) = 0;
    // We finish process this node.
    virtual void FinishProcessNode(ObjectId nodeId) = 0;
    
    virtual ~IEnumStrategy() {}
};

/**
 * Graph base specification.
 *
 */
class IGraph
{
public:
    // Get Nodes count.
    virtual IndexType GetNodesCount() const = 0;
    // Get Edges count.
    virtual IndexType GetEdgesCount() const = 0;
    // Get node of this graph.
    virtual ObjectId GetNode(IndexType index) const = 0;
    // Get node by id.
    virtual ObjectId GetNode(const char* nodeId) const = 0;
    // Get connected graph count.
    virtual IndexType GetConnectedNodes(ObjectId source) const = 0;
    // Get connected graph for this graph.
    virtual ObjectId GetConnectedNode(ObjectId source, IndexType index) const = 0;
    // Is edge exists.
    virtual bool AreNodesConnected(ObjectId source, ObjectId target) const = 0;
    // Return graph string Id.
    virtual bool GetNodeStrId(ObjectId node, char* outBuffer, IndexType bufferSize) const = 0;
    // Is edge exists in input graph. It is not the same with IsEgdeExists. 
    virtual bool IsEgdeExists(ObjectId source, ObjectId target) const = 0;
    // Get weight real type
    virtual EdgeWeightType GetEdgeWeightType() const = 0;
    // Create copy of graph.
    virtual IGraph* MakeBaseCopy(GraphCopyType type) const = 0;
    // Is graph directed or not.
    virtual bool IsDirected() const = 0;
    // Run DFS and call callbacks.
    virtual void ProcessDFS(IEnumStrategy* pEnumStrategy, ObjectId startedNode) const = 0;
    
    virtual ~IGraph() {};
};


class IGraphInt : public IGraph
{
public:
    // Get Egde weight of int graph.
    virtual IntWeightType GetEdgeWeight(ObjectId source, ObjectId target) const = 0;
    
    // Create copy of graph.
    virtual IGraphInt* MakeCopy(GraphCopyType type) const = 0;
};

class IGraphFloat : public IGraph
{
public:
    // Get Egde weight of int graph.
    virtual FloatWeightType GetEdgeWeight(ObjectId source, ObjectId target) const = 0;
    
    // Create copy of graph.
    virtual IGraphFloat* MakeCopy(GraphCopyType type) const = 0;
};


// Base typedaef
typedef std::shared_ptr<IGraph> GraphPtr;

