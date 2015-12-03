/**
 *  Interface for graph.
 *
 */

#pragma once

typedef void* ObjectId;

class IGraph
{
public:
  // Get Nodes count.
  virtual unsigned int GetNodesCount() = 0;
  // Get Edges count.
  virtual unsigned int GetEdgesCount() = 0;
  // Get node of this graph.
  virtual ObjectId GetNode(int index) = 0;
  // Get connected graph count.
  virtual unsigned int GetConnectedGraphs(ObjectId source) = 0;
  // Get connected graph for this graph.
  virtual ObjectId GetConnectedGraph(ObjectId source, int index) = 0;
  // Is edge exists.
  virtual bool IsEgdeExists(ObjectId source, ObjectId target) = 0;
  // Get Egde weight. TODO: float.
  virtual int GetEdgeWeight(ObjectId source, ObjectId target) = 0;
  // Return graph string Id.
  virtual bool GetNodeStrId(ObjectId node, char* outBuffer, int bufferSize) = 0;
  // Is edge exists in input graph.
  virtual bool IsEgdeExistsInInput(ObjectId source, ObjectId target) = 0;
};
