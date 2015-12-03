#pragma once

#include <string>
#include <vector>
#include "IGraph.h"
#include "YString.h"

/**
 * Main Graph class.
 *
 */
class Graph : public IGraph
{
public:

  // Node sturct
  struct Node
  {
    String id;

    Node(String id)
    {
      this->id = id;
    }
    std::vector<Node*> targets;
  };

  // Edge struct
  struct Edge
  {
    String id;
    Node* source;
    Node* target;
    bool  direct;
    double weight;
    Edge(String id, Node* source, Node* target, bool direct, double weight)
    {
      this->id = id;
      this->source = source;
      this->target = target;
      this->direct = direct;
      this->weight = weight;
    }
  };

  Graph(void);
  virtual ~Graph(void);

  // Load from GraphML format.
  bool LoadFromGraphML(const char * pBuffer, int bufferSize);

  // Clear Graph.
  void Clear();

  // IGraph
  // Get Nodes count.
  virtual unsigned int GetNodesCount();
  // Get Edges count.
  virtual unsigned int GetEdgesCount();
  // Get node of this graph.
  virtual ObjectId GetNode(int index);
  // Get connected graph count.
  virtual unsigned int GetConnectedGraphs(ObjectId source);
  // Get connected graph for this graph.
  virtual ObjectId GetConnectedGraph(ObjectId source, int index);
  // Is edge exists.
  virtual bool IsEgdeExists(ObjectId source, ObjectId target);
  // Get Egde weight. TODO: float.
  virtual int GetEdgeWeight(ObjectId source, ObjectId target);
  // Return graph string Id.
  virtual bool GetNodeStrId(ObjectId node, char* outBuffer, int bufferSize);
  // Is edge exists in input graph.
  virtual bool IsEgdeExistsInInput(ObjectId source, ObjectId target);

  // Find Node by Id
  Node* FindNode(const String& id);
protected:

  // Find Node by id in vector.
  Node* FindNode(const String& id, const std::vector<Node*>& nodes);

  // Find element in vector.
  template <typename T> bool Has(const std::vector<T>& vector, const T& value);

  // Is valid Object id.
  bool IsValidNodeId(ObjectId id);

  // Is edge exists.
  Edge* FindEdge(ObjectId source, ObjectId target);

  // List of graph.
  std::vector<Node*> m_nodes;
  std::vector<Edge*> m_edges;

};
