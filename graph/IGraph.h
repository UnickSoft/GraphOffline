/**
 *  Interface for graph.
 *
 */

#pragma once

#include <cstdint>
#include <memory>
#include <cassert>

// Index value
typedef uint32_t IndexType;
// Id of any object
using ObjectId = int32_t;
// Integer edge weight
typedef int32_t IntWeightType;
// Float edge weight
typedef double FloatWeightType;

// Type of edges wieght.
enum EdgeWeightType {WT_INT = 0, WT_FLOAT};

// Graph copy type.
enum GraphCopyType {GCT_COPY = 0,
                    GTC_MAKE_UNDIRECTED,
                    GTC_INVERSE,
                    GTC_REMOVE_SELF_LOOP,
                    GTC_MULTI_TO_COMMON_GRAPH_MINIMAL_EDGES,
                    GTC_MULTI_TO_COMMON_GRAPH_SUM_EDGES,
                    GTC_REMOVE_NEGATIVE,
                    GTC_COMPLEMENT};

/**
 * Call back for enum nodes methods (DFS, BSF).
 *
 */
class IEnumStrategy
{
public:

    // Default enum strategy.
    // DES_NONE - has no default enum strategy.
    // DES_NODE - will enum nodes only one time.
    // DES_EDGE - will enum edge only one time.
    enum DefaultEnumStrategy {DES_NONE = 0, DES_NODE, DES_EDGE};

    // We started process this node.
    virtual void StartProcessNode(ObjectId nodeId) = 0;
    // @return true if we need to process this child node.
    virtual bool NeedProcessChild(ObjectId nodeId, ObjectId childId, ObjectId edgeId) = 0;
    // We finish process this node.
    virtual void FinishProcessNode(ObjectId nodeId) = 0;
    // Default Strategy.
    virtual DefaultEnumStrategy GetDefaultStrategy() = 0;

    // We started process this edge.
    virtual void StartProcessEdge(ObjectId edgeId) = 0;
    // We finish process this edge.
    virtual void FinishProcessEdge(ObjectId edgeId) = 0;

    virtual ~IEnumStrategy() {}
};

/**
 * Graph base specification.
 *
 */
class IGraph
{
public:
    // Edge data
    struct NodePair
    {
      ObjectId source;
      ObjectId target;
    };

    // Get Nodes count.
    virtual IndexType GetNodesCount() const = 0;
    // Get Edges count.
    virtual IndexType GetEdgesCount() const = 0;
    // Get node of this graph.
    virtual ObjectId GetNode(IndexType index) const = 0;
    // Get node by id.
    virtual ObjectId GetNode(const char* nodeId) const = 0;
    // Return edge Id
    virtual ObjectId GetEdge(ObjectId source, ObjectId target) const = 0;
    // Get connected graph count.
    virtual IndexType GetConnectedNodes(ObjectId source) const = 0;
    // Get connected graph for this graph.
    virtual ObjectId GetConnectedNode(ObjectId source, IndexType index) const = 0;
    // Get connected vertex index.
    virtual IndexType GetConnectedNodeIndex(ObjectId source, IndexType index) const = 0;
    // Is edge exists.
    virtual bool AreNodesConnected(ObjectId source, ObjectId target) const = 0;
    // Return graph string Id.
    virtual bool GetNodeStrId(ObjectId node, char* outBuffer, IndexType bufferSize) const = 0;
    // Return edge string Id.
    virtual bool GetEdgeStrId(ObjectId edge, char* outBuffer, IndexType bufferSize) const = 0;
    // Is edge exists in input graph. It is not the same with AreNodesConnected.
    virtual bool IsEgdeExists(ObjectId source, ObjectId target, bool onlyInSourceGraph = true) const = 0;
    // Get weight real type
    virtual EdgeWeightType GetEdgeWeightType() const = 0;
    // Create copy of graph.
    virtual IGraph* MakeBaseCopy(GraphCopyType type) const = 0;
    // Has directed edges or not.
    virtual bool HasDirected() const = 0;
    // Has undirected edges or not.
    virtual bool HasUndirected() const = 0;
    // Has multi graph
    virtual bool IsMultiGraph() const = 0;
    // Run DFS and call callbacks.
    virtual void ProcessDFS(IEnumStrategy* pEnumStrategy, ObjectId startedNode) const = 0;
    // Remove edge from Graph. For undirected Graph it removes source -> target or target -> source
    virtual void RemoveEdge(ObjectId source, ObjectId target) = 0;
    // How many nodes are source for this node.
    virtual IndexType GetSourceNodesNumber(ObjectId source) = 0;
    // Add edge
    virtual bool AddEdge(ObjectId source, ObjectId target, bool direct, const FloatWeightType& weight) = 0;
    // Add node
    virtual ObjectId AddNode(bool fake) = 0;
    // Is fake node or not.
    virtual bool IsFakeNode(ObjectId source) = 0;
    // Return graph struct
    virtual const char* PrintGraph() const = 0;
    // Remove node and all connected edges.
    virtual void RemoveNode(ObjectId source) = 0;
    // Edge id.
    virtual ObjectId GetEdge(IndexType index) const = 0;
    // Edge connected nodes
    virtual NodePair GetEdgeData(IndexType index) const = 0;
    // Edge connected nodes
    virtual NodePair GetEdgeData(ObjectId index) const = 0;

    virtual ~IGraph() {};
};

class IGraphInt: public virtual IGraph
{
public:
    // Get Egde weight of int graph.
    virtual IntWeightType GetEdgeWeight(ObjectId source, ObjectId target) const = 0;
    // Create copy of graph.
    virtual IGraphInt* MakeCopy(GraphCopyType type) const = 0;
};

class IGraphFloat: public virtual IGraph
{
public:
    // Get Egde weight of int graph.
    virtual FloatWeightType GetEdgeWeight(ObjectId source, ObjectId target) const = 0;

    // Create copy of graph.
    virtual IGraphFloat* MakeCopy(GraphCopyType type) const = 0;
};

class IMultiGraph : public virtual IGraph
{
public:
    // Number of edges between nodes
    virtual IndexType GetEdgesNumber(ObjectId source, ObjectId target) const = 0;
    // Return Id of edge.
    virtual ObjectId GetEdge(ObjectId source, ObjectId target, const IndexType & index) const = 0;
    // Return edge string Id.
    virtual bool GetEdgeStrId(ObjectId edge, char* outBuffer, IndexType bufferSize) const = 0;
    // Remove by id
    virtual void RemoveEdgeByID(ObjectId edgeId) = 0;

protected:

    virtual ObjectId GetEdge(ObjectId source, ObjectId target) const = 0;
};

class IMultiGraphInt: public virtual IMultiGraph
{
public:
    // Get Egde weight of int graph.
    virtual IntWeightType GetEdgeWeight(ObjectId edgeId) const = 0;
    // Create copy of graph.
    virtual IMultiGraphInt* MakeCopy(GraphCopyType type) const = 0;
};

class IMultiGraphFloat: public virtual IMultiGraph
{
public:
    // Get Egde weight of int graph.
    virtual FloatWeightType GetEdgeWeight(ObjectId edgeId) const = 0;

    // Create copy of graph.
    virtual IMultiGraphFloat* MakeCopy(GraphCopyType type) const = 0;
};

// Base typedaef
typedef std::shared_ptr<IGraph> GraphPtr;
