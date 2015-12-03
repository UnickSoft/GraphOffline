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
  DijkstraShortPath (ObjectId source, ObjectId target, IGraph* pGraph);
  virtual ~DijkstraShortPath ();

  // Calculate algorithm.
  virtual void Calculate();
  // Hightlight nodes count.
  virtual unsigned int GetHightlightNodesCount();
  // Hightlight node.
  virtual ObjectId GetHightlightNode(int index);
  // Hightlight edges count.
  virtual unsigned int GetHightlightEdgesCount();
  // Hightlight edge.
  virtual NodesEdge GetHightlightEdge(int index);
  // Get result.
  virtual int GetResult();
  // Get propery
  virtual int GetProperty(ObjectId, const char* name);

protected:
  ObjectId m_source;
  ObjectId m_target;
  IGraph*  m_pGraph;
  std::vector<ObjectId> m_path;
  std::unordered_map<ObjectId, int> m_lowestDistance;

  int m_result;
};
