/**
 *  Interface for graph.
 *
 */

#pragma once

#include <cstdint>

// Id of any object
typedef void* ObjectId;
// Index value
typedef uint32_t IndexType;
// Integer edge weight
typedef int32_t IntWeightType;

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
    // Get Egde weight. TODO: float.
    virtual IntWeightType GetEdgeWeight(ObjectId source, ObjectId target) const = 0;
    // Return graph string Id.
    virtual bool GetNodeStrId(ObjectId node, char* outBuffer, IndexType bufferSize) const = 0;
    // Is edge exists in input graph. It is not the same with IsEgdeExists. 
    virtual bool IsEgdeExists(ObjectId source, ObjectId target) const = 0;
    
    virtual ~IGraph() {};
};
