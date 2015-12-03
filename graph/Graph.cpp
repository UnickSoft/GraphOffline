

#include "Graph.h"
#include "pugixml.hpp"
#include <algorithm>

// Find Edge funtor.
struct FindEdgeFunctor
{
  FindEdgeFunctor(Graph::Node* sourceNode, Graph::Node* targetNode)
  {
    this->sourceNode = sourceNode;
    this->targetNode = targetNode;
  }
  bool operator()(Graph::Edge* edge)
  {
    return (edge->source == sourceNode && edge->target == targetNode) ||
      (edge->source == targetNode && edge->target == sourceNode && !edge->direct);
  }
  Graph::Node* sourceNode;
  Graph::Node* targetNode;
};

Graph::Graph(void)
{

}

Graph::~Graph(void)
{
  Clear();
}


// Load from GraphML format.
bool Graph::LoadFromGraphML(const char * pBuffer, int bufferSize)
{
  Clear();
  bool res = false;
  pugi::xml_document doc;

  pugi::xml_parse_result result = doc.load_buffer(pBuffer, bufferSize);

  if (result)
  {
    pugi::xml_node graphml = doc.child("graphml");

    if (graphml)
    {
      double defaultWeight = 1.0;
      String weightId = "";

      pugi::xml_object_range<pugi::xml_named_node_iterator> childrenKey = graphml.children("key");

      for (pugi::xml_named_node_iterator key = childrenKey.begin(); key != childrenKey.end(); key++)
      {
        if (String(key->attribute("attr.name").value()) == String("weight"))
        {
          pugi::xml_node defaultWeightNode = key->child("default");
          if (defaultWeightNode)
          {
            defaultWeight = defaultWeightNode.text().as_double(1.0);
            weightId = key->attribute("id").value();
            break;
          }
        }
      }


      pugi::xml_node graph = graphml.child("graph");
      pugi::xml_attribute xmlDefaultEdge  = graph.attribute("edgedefault");
      bool bDefaultDirect = false;
      if (xmlDefaultEdge)
      {
        bDefaultDirect = (String(xmlDefaultEdge.value()) == String("directed"));
      }
      if (!graph.empty())
      {
        // Enum nodes.
        pugi::xml_object_range<pugi::xml_named_node_iterator> nodeList = graph.children("node");
        for (pugi::xml_named_node_iterator node = nodeList.begin(); node != nodeList.end(); node++)
        {
          m_nodes.push_back(new Node(String(node->attribute("id").value())));
        }

        // Enum edges.
        pugi::xml_object_range<pugi::xml_named_node_iterator> edgeList = graph.children("edge");
        for (pugi::xml_named_node_iterator edge = edgeList.begin(); edge != edgeList.end(); edge++)
        {
          Node* sourceNode = FindNode(edge->attribute("source").value());
          Node* targetNode = FindNode(edge->attribute("target").value());
          pugi::xml_attribute directXML = edge->attribute("directed");
          bool direct = bDefaultDirect;
          if (directXML)
          {
            direct = directXML.as_bool();
          }
          double weight = defaultWeight;
          pugi::xml_node edgeData = edge->child("data");
          if (edgeData && String(edgeData.attribute("key").value()) == String(weightId))
          {
            weight = edgeData.text().as_double();
          }

          m_edges.push_back(
            new Edge(String(edge->attribute("id").value()), sourceNode, targetNode, direct, weight));

          // Add to nodes.
          if (FindNode(targetNode->id, sourceNode->targets) == NULL)
          {
            sourceNode->targets.push_back(targetNode);
          }

          if (!direct)
          {
            if (FindNode(sourceNode->id, targetNode->targets) == NULL)
            {
              targetNode->targets.push_back(sourceNode);
            }
          }
        }
        res = true;
      }
    }
  }

  return res;
}


void Graph::Clear()
{
  for (int i = 0; i < m_nodes.size(); i++)
  {
    delete m_nodes[i];
    m_nodes[i] = NULL;
  }
  for (int i = 0; i < m_edges.size(); i++)
  {
    delete m_edges[i];
    m_edges[i] = NULL;
  }

  m_nodes.clear();
  m_edges.clear();
}

Graph::Node* Graph::FindNode(const String& id)
{
  return FindNode(id, m_nodes);
}

Graph::Node* Graph::FindNode(const String& id, const std::vector<Node*>& nodes)
{
  Node* node = NULL;
  for (int i = 0; i < nodes.size(); i++)
  {
    if (nodes[i] && nodes[i]->id == id)
    {
      node = nodes[i];
      break;
    }
  }

  return node;
}

// Get Nodes count.
unsigned int Graph::GetNodesCount()
{
  return m_nodes.size();
}

ObjectId Graph::GetNode(int index)
{
  return m_nodes[index];
}


// Get Edges count.
unsigned int Graph::GetEdgesCount()
{
  return m_edges.size();
}

// Get connected graph count.
unsigned int Graph::GetConnectedGraphs(ObjectId source)
{
  unsigned int res = 0;
  if (IsValidNodeId(source))
  {
    res = ((Node*)source)->targets.size();
  }
  return res;
}

// Get connected graph for this graph.
ObjectId Graph::GetConnectedGraph(ObjectId source, int index)
{
  ObjectId res = 0;
  if (IsValidNodeId(source))
  {
    res = ((Node*)source)->targets[index];
  }
  return res;
}

// Is edge exists.
bool Graph::IsEgdeExists(ObjectId source, ObjectId target)
{
  bool res = false;
  if (IsValidNodeId(source) && IsValidNodeId(target))
  {
    res = Has(((Node*)source)->targets, (Node *)target);
  }
  return res;
}

// Is edge exists in input graph.
bool Graph::IsEgdeExistsInInput(ObjectId source, ObjectId target)
{
  bool res = false;
  if (IsValidNodeId(source) && IsValidNodeId(target))
  {
    Node* sourceNode = (Node*)source;
    Node* targetNode = (Node*)target;

    for (int i = 0; i < m_edges.size(); i++)
    {
      if (m_edges[i]->source == sourceNode && m_edges[i]->target == targetNode)
      {
        res = true;
        break;
      }
    }
  }
  return res;
}

// Get Egde weight. TODO: float.
int Graph::GetEdgeWeight(ObjectId source, ObjectId target)
{
  int res = 1;
  Edge* edge = FindEdge(source, target);
  if (edge)
  {
    res = (int)edge->weight;
  }
  return res;
}

// Return graph string Id.
bool Graph::GetNodeStrId(ObjectId node, char* outBuffer, int bufferSize)
{
  bool res = false;
  if (IsValidNodeId(node))
  {
	  res = ((Node*)(node))->id.PrintLocale(outBuffer, bufferSize) > 0;
  }
  return res;
}

bool Graph::IsValidNodeId(ObjectId id)
{
  return Has(m_nodes, (Node *)id);
}

template <typename T> bool Graph::Has(const std::vector<T>& vector, const T& value)
{
  return std::find(vector.begin(), vector.end(), value) != vector.end();
}

Graph::Edge* Graph::FindEdge(ObjectId source, ObjectId target)
{
  Edge* res = NULL;
  if (IsValidNodeId(source) && IsValidNodeId(target))
  {
    Node* sourceNode = (Node*)source;
    Node* targetNode = (Node*)target;

    std::vector<Edge*>::iterator edgeIterator = 
      std::find_if(m_edges.begin(), m_edges.end(), FindEdgeFunctor(sourceNode, targetNode));

    if (edgeIterator != m_edges.end())
    {
      res = *edgeIterator;
    }
  }
  return res;

}