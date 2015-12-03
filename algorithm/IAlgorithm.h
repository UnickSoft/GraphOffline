/**
 *
 *  Interface for algorithm.
 *
 */


#pragma once

#include "IGraph.h"

// Edge struct.
struct NodesEdge
{
  ObjectId source;
  ObjectId target;
};

// If path is not avalible, it may have result.
#define INFINITY_PATH_INT 0x7FFFFFFF

class IAlgorithmEngine
{
public:
  // Calculate algorithm.
  virtual void Calculate() = 0;
};

class IAlgorithmResult
{
public:
  // Calculate algorithm.
  virtual void Calculate() = 0;
  // Hightlight nodes count.
  virtual unsigned int GetHightlightNodesCount() = 0;
  // Hightlight node.
  virtual ObjectId GetHightlightNode(int index) = 0;
  // Hightlight edges count.
  virtual unsigned int GetHightlightEdgesCount() = 0;
  // Hightlight edge.
  virtual NodesEdge GetHightlightEdge(int index) = 0;
  // Get result.
  virtual int GetResult() = 0;
  // Get propery
  virtual int GetProperty(ObjectId, const char* name) = 0;
};

class IAlgorithm : public IAlgorithmEngine, public IAlgorithmResult 
{
};
