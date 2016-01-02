

#include "Graph.h"
#include "pugixml.hpp"
#include <algorithm>
#include <math.h>
#include "Logger.h"

// Find Edge funtor.
template<class _WeightInterface, typename _WeightType> class FindEdgeFunctor
{
public:
  FindEdgeFunctor(typename Graph<_WeightInterface, _WeightType>::Node* sourceNode, typename Graph<_WeightInterface, _WeightType>::Node* targetNode)
  {
    this->sourceNode = sourceNode;
    this->targetNode = targetNode;
  }
  bool operator()(typename Graph<_WeightInterface, _WeightType>::EdgePtr edge)
  {
    return (edge->source == sourceNode && edge->target == targetNode) ||
      (edge->source == targetNode && edge->target == sourceNode && !edge->direct);
  }
  typename Graph<_WeightInterface, _WeightType>::Node* sourceNode;
  typename Graph<_WeightInterface, _WeightType>::Node* targetNode;
};

template<class WeightInterface, typename WeightType> Graph<WeightInterface, WeightType>::Graph(void)
{
    m_weightType = WT_INT;
    m_autoIncIndex = 1; // 0 is invalid value.
}

template<class WeightInterface, typename WeightType> Graph<WeightInterface, WeightType>::~Graph(void)
{
  Clear();
}


// Load from GraphML format.
template<class WeightInterface, typename WeightType> bool Graph<WeightInterface, WeightType>::LoadFromGraphML(const char * pBuffer, uint32_t bufferSize)
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
                        m_weightType = IsDouble(defaultWeight) || m_weightType == WT_FLOAT ? WT_FLOAT : WT_INT;
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
                    m_nodes.push_back(NodePtr(new Node(String(node->attribute("id").value()), GetNextId())));
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
                        m_weightType = IsDouble(weight) || m_weightType == WT_FLOAT ? WT_FLOAT : WT_INT;
                    }
                    
                    m_edges.push_back(
                                      EdgePtr(new Edge(String(edge->attribute("id").value()), sourceNode, targetNode, direct, weight, GetNextId())));
                    
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


template<class WeightInterface, typename WeightType> void Graph<WeightInterface, WeightType>::Clear()
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

template<class WeightInterface, typename WeightType> typename Graph<WeightInterface, WeightType>::NodePtr Graph<WeightInterface, WeightType>::FindNode(const String& id) const
{
  return FindNode(id, m_nodes);
}

template<class WeightInterface, typename WeightType> template <typename T> T Graph<WeightInterface, WeightType>::FindNode(const String& id, const std::vector<T>& nodes) const
{
  T node = T(nullptr);
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

template<class WeightInterface, typename WeightType> template <typename T> T Graph<WeightInterface, WeightType>::FindNode(ObjectId objectId, const std::vector<T>& nodes) const
{
    T node = T(nullptr);
    //Node* nodeObject = (Node*)objectId;
    for (int i = 0; i < nodes.size(); i++)
    {
        if (nodes[i]->privateId == objectId)
        {
            node = nodes[i];
            break;
        }
    }
    
    return node;
}

// Get Nodes count.
template<class WeightInterface, typename WeightType> IndexType Graph<WeightInterface, WeightType>::GetNodesCount() const
{
  return m_nodes.size();
}

template<class WeightInterface, typename WeightType> ObjectId Graph<WeightInterface, WeightType>::GetNode(IndexType index) const
{
  return m_nodes[index].get()->privateId;
}

template<class WeightInterface, typename WeightType>  ObjectId Graph<WeightInterface, WeightType>::GetNode(const char* nodeId) const
{
    NodePtr res = FindNode(String().FromLocale(nodeId));
    return res ? res.get()->privateId : 0;
}


// Get Edges count.
template<class WeightInterface, typename WeightType>  IndexType Graph<WeightInterface, WeightType>::GetEdgesCount() const
{
  return m_edges.size();
}

// Get connected graph count.
template<class WeightInterface, typename WeightType>  IndexType Graph<WeightInterface, WeightType>::GetConnectedNodes(ObjectId source) const
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
template<class WeightInterface, typename WeightType>  ObjectId Graph<WeightInterface, WeightType>::GetConnectedNode(ObjectId source, IndexType index) const
{
    ObjectId res = 0;
    NodePtr sourcePtr;
    if (IsValidNodeId(source, sourcePtr))
    {
        res = sourcePtr->targets[index]->privateId;
    }
    return res;
}

// Is edge exists.
template<class WeightInterface, typename WeightType>  bool Graph<WeightInterface, WeightType>::AreNodesConnected(ObjectId source, ObjectId target) const
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
template<class WeightInterface, typename WeightType>  bool Graph<WeightInterface, WeightType>::IsEgdeExists(ObjectId source, ObjectId target) const
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
template<class WeightInterface, typename WeightType> WeightType Graph<WeightInterface, WeightType>::GetEdgeWeight(ObjectId source, ObjectId target) const
{
    WeightType res = 1;
    EdgePtr edge = FindEdge(source, target);
    assert(edge);

    if (edge)
    {
        res = (WeightType)edge->weight;
    }
    else
    {
        LOG_WARNING("Cannot find edge from " << source << " to " << target);
    }
    
    return res;
}

// Return graph string Id.
template<class WeightInterface, typename WeightType>  bool Graph<WeightInterface, WeightType>::GetNodeStrId(ObjectId node, char* outBuffer, IndexType bufferSize) const
{
    bool res = false;
    NodePtr nodePtr;
    if (IsValidNodeId(node, nodePtr))
    {
        res = nodePtr->id.PrintLocale(outBuffer, bufferSize) > 0;
    }
    return res;
}

template<class WeightInterface, typename WeightType> bool Graph<WeightInterface, WeightType>::IsValidNodeId(ObjectId id, NodePtr& ptr) const
{
    ptr = FindNode(id, m_nodes);
    return ptr != nullptr;
}

template<class WeightInterface, typename WeightType> template <typename T> bool Graph<WeightInterface, WeightType>::Has(const std::vector<T>& vector, const T& value) const
{
  return std::find(vector.begin(), vector.end(), value) != vector.end();
}

template<class WeightInterface, typename WeightType> template <typename T1, typename T2> bool Graph<WeightInterface, WeightType>::Has(const std::vector<T1>& vector, const T2& value) const
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

template<class WeightInterface, typename WeightType> typename Graph<WeightInterface, WeightType>::EdgePtr Graph<WeightInterface, WeightType>::FindEdge(ObjectId source, ObjectId target) const
{
    EdgePtr res;
    NodePtr sourcePtr, targetPtr;
    if (IsValidNodeId(source, sourcePtr) && IsValidNodeId(target, targetPtr))
    {
        Node* sourceNode = (Node*)source;
        Node* targetNode = (Node*)target;
        
        auto edgeIterator =
        std::find_if(m_edges.begin(), m_edges.end(), FindEdgeFunctor<WeightInterface, WeightType>(sourcePtr.get(), targetPtr.get()));
        
        if (edgeIterator != m_edges.end())
        {
            res = *edgeIterator;
        }
    }
    
    return res;
}


template<class WeightInterface, typename WeightType> bool Graph<WeightInterface, WeightType>::IsDouble(double value)
{
    return fabs(value - floor(value)) > 1E-5;
}

template<class WeightInterface, typename WeightType> EdgeWeightType Graph<WeightInterface, WeightType>::GetEdgeWeightType() const
{
    return m_weightType;
}


// Create copy of graph.
template<class WeightInterface, typename WeightType> WeightInterface* Graph<WeightInterface, WeightType>::MakeCopy(GraphCopyType type)
{
    return this;
}

template<class WeightInterface, typename WeightType> IndexType Graph<WeightInterface, WeightType>::GetNextId()
{
    return ++m_autoIncIndex;
}