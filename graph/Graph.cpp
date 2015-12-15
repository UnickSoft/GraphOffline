

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
  bool operator()(Graph::EdgePtr edge)
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
bool Graph::LoadFromGraphML(const char * pBuffer, uint32_t bufferSize)
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
                    m_nodes.push_back(NodePtr(new Node(String(node->attribute("id").value()))));
                }
                
                // Enum edges.
                pugi::xml_object_range<pugi::xml_named_node_iterator> edgeList = graph.children("edge");
                for (pugi::xml_named_node_iterator edge = edgeList.begin(); edge != edgeList.end(); edge++)
                {
                    NodePtr sourceNode = FindNode(edge->attribute("source").value());
                    NodePtr targetNode = FindNode(edge->attribute("target").value());
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
                                      EdgePtr(new Edge(String(edge->attribute("id").value()), sourceNode, targetNode, direct, weight)));
                    
                    // Add to nodes.
                    if (!FindNode(targetNode->id, sourceNode->targets))
                    {
                        sourceNode->targets.push_back(targetNode.get());
                    }
                    
                    if (!direct)
                    {
                        if (!FindNode(sourceNode->id, targetNode->targets))
                        {
                            targetNode->targets.push_back(sourceNode.get());
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
/*
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
*/

  m_nodes.clear();
  m_edges.clear();
}

Graph::NodePtr Graph::FindNode(const String& id) const
{
  return FindNode(id, m_nodes);
}

template <typename T> T Graph::FindNode(const String& id, const std::vector<T>& nodes) const
{
  T node = nullptr;
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

template <typename T> T Graph::FindNode(ObjectId objectId, const std::vector<T>& nodes) const
{
    T node = nullptr;
    Node* nodeObject = (Node*)objectId;
    for (int i = 0; i < nodes.size(); i++)
    {
        if (nodes[i] == nodeObject)
        {
            node = nodes[i];
            break;
        }
    }
    
    return node;
}

// Get Nodes count.
IndexType Graph::GetNodesCount() const
{
  return m_nodes.size();
}

ObjectId Graph::GetNode(IndexType index) const
{
  return m_nodes[index].get();
}

ObjectId Graph::GetNode(const char* nodeId) const
{
    NodePtr res = FindNode(String().FromLocale(nodeId));
    return res ? res.get() : nullptr;
}


// Get Edges count.
IndexType Graph::GetEdgesCount() const
{
  return m_edges.size();
}

// Get connected graph count.
IndexType Graph::GetConnectedNodes(ObjectId source) const
{
    IndexType res = 0;
    NodePtr nodePtr;
    if (IsValidNodeId(source, nodePtr) && nodePtr)
    {
        res = nodePtr->targets.size();
    }
    return res;
}

// Get connected graph for this graph.
ObjectId Graph::GetConnectedNode(ObjectId source, IndexType index) const
{
    ObjectId res = 0;
    NodePtr sourcePtr;
    if (IsValidNodeId(source, sourcePtr))
    {
        res = sourcePtr->targets[index];
    }
    return res;
}

// Is edge exists.
bool Graph::AreNodesConnected(ObjectId source, ObjectId target) const
{
  bool res = false;
    NodePtr sourcePtr, targetPtr;
  if (IsValidNodeId(source, sourcePtr) && IsValidNodeId(target, targetPtr))
  {
    res = Has<Node*, NodePtr>(sourcePtr->targets, targetPtr);
  }
  return res;
}

// Is edge exists in input graph.
bool Graph::IsEgdeExists(ObjectId source, ObjectId target) const
{
  bool res = false;
    NodePtr sourcePtr, targetPtr;
  if (IsValidNodeId(source, sourcePtr) && IsValidNodeId(target, targetPtr))
  {
    for (int i = 0; i < m_edges.size(); i++)
    {
      if (m_edges[i]->source == sourcePtr && m_edges[i]->target == targetPtr)
      {
        res = true;
        break;
      }
    }
  }
  return res;
}

// Get Egde weight. TODO: float.
IntWeightType Graph::GetEdgeWeight(ObjectId source, ObjectId target) const
{
  IntWeightType res = 1;
  EdgePtr edge = FindEdge(source, target);
  if (edge)
  {
    res = (int)edge->weight;
  }
  return res;
}

// Return graph string Id.
bool Graph::GetNodeStrId(ObjectId node, char* outBuffer, IndexType bufferSize) const
{
    bool res = false;
    NodePtr nodePtr;
    if (IsValidNodeId(node, nodePtr))
    {
        res = nodePtr->id.PrintLocale(outBuffer, bufferSize) > 0;
    }
    return res;
}

bool Graph::IsValidNodeId(ObjectId id, NodePtr& ptr) const
{
    ptr = FindNode(id, m_nodes);
    return ptr != nullptr;
}

template <typename T> bool Graph::Has(const std::vector<T>& vector, const T& value) const
{
  return std::find(vector.begin(), vector.end(), value) != vector.end();
}

template <typename T1, typename T2> bool Graph::Has(const std::vector<T1>& vector, const T2& value) const
{
    bool res = false;
    for (const T1& item : vector)
    {
        if (value == item)
        {
            res = true;
            break;
        }
        
    }
    return res;
}

Graph::EdgePtr Graph::FindEdge(ObjectId source, ObjectId target) const
{
    EdgePtr res = NULL;
    NodePtr sourcePtr, targetPtr;
    if (IsValidNodeId(source, sourcePtr) && IsValidNodeId(target, targetPtr))
    {
        Node* sourceNode = (Node*)source;
        Node* targetNode = (Node*)target;
        
        auto edgeIterator =
        std::find_if(m_edges.begin(), m_edges.end(), FindEdgeFunctor(sourceNode, targetNode));
        
        if (edgeIterator != m_edges.end())
        {
            res = *edgeIterator;
        }
    }
    
    return res;
}

