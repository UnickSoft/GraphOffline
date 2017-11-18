

#include "Graph.h"
#include "pugixml.hpp"
#include <algorithm>
#include <math.h>
#include "Logger.h"
#include <unordered_set>

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


// Base Enum to exclude infinity loop using Nodes.
class BaseNodeEnumStrategy : public IEnumStrategy
{
public:
    BaseNodeEnumStrategy (IEnumStrategy* pUserStrategy) : m_pUserStrategy(pUserStrategy) {;}
    
    virtual void StartProcessNode(ObjectId nodeId)
    {
        processedNode.insert(nodeId);
        m_pUserStrategy->StartProcessNode(nodeId);
    }
    
    virtual bool NeedProcessChild(ObjectId nodeId, ObjectId childId, ObjectId edge)
    {
        return m_pUserStrategy->NeedProcessChild(nodeId, childId, edge) &&
            processedNode.find(childId) == processedNode.end();
    }
    
    virtual void FinishProcessNode(ObjectId nodeId)
    {
        m_pUserStrategy->FinishProcessNode(nodeId);
    }
    
    // Default Strategy.
    virtual DefaultEnumStrategy GetDefaultStrategy()
    {
        return DES_NONE;
    }
    
protected:
    IEnumStrategy* m_pUserStrategy;
    std::unordered_set<ObjectId> processedNode;
};

// Base Enum to exclude infinity loop using Edges.
class BaseEdgeEnumStrategy : public IEnumStrategy
{
public:
    BaseEdgeEnumStrategy (IEnumStrategy* pUserStrategy) : m_pUserStrategy(pUserStrategy) {;}
    
    virtual void StartProcessNode(ObjectId nodeId)
    {
        m_pUserStrategy->StartProcessNode(nodeId);
    }
    
    virtual bool NeedProcessChild(ObjectId nodeId, ObjectId childId, ObjectId edge)
    {
        bool res = m_pUserStrategy->NeedProcessChild(nodeId, childId, edge) &&
        (processedEdge.find(edge) == processedEdge.end());
        if (res)
        {
            processedEdge.insert(edge);
        }
        return res;
    }
    
    virtual void FinishProcessNode(ObjectId nodeId)
    {
        m_pUserStrategy->FinishProcessNode(nodeId);
    }
    
    // Default Strategy.
    virtual DefaultEnumStrategy GetDefaultStrategy()
    {
        return DES_NONE;
    }
    
protected:
    IEnumStrategy* m_pUserStrategy;
    std::unordered_set<ObjectId> processedEdge;
};

// 0 is invalid value. 1000 - because it is not the same as index.
template<class WeightInterface, typename WeightType> IndexType Graph<WeightInterface, WeightType>::m_autoIncIndex = 1000;


template<class WeightInterface, typename WeightType> Graph<WeightInterface, WeightType>::Graph(void)
{
    m_weightType = WT_INT;
    m_hasDirected   = false;
    m_hasUndirected = false;
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
                    AddNode(String(node->attribute("id").value()), GetNextId(), false);
                    //m_nodes.push_back(NodePtr(new Node(String(node->attribute("id").value()), GetNextId())));
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
                    
                    AddEdge(String(edge->attribute("id").value()), sourceNode->privateId, targetNode->privateId, direct, weight, GetNextId());
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
        res = nodePtr->GetTargets().size();
    }
    
    assert(nodePtr);
    
    return res;
}

// Get connected graph for this graph.
template<class WeightInterface, typename WeightType>  ObjectId Graph<WeightInterface, WeightType>::GetConnectedNode(ObjectId source, IndexType index) const
{
    ObjectId res = 0;
    NodePtr sourcePtr;
    if (IsValidNodeId(source, sourcePtr))
    {
        res = sourcePtr->GetTargets().at(index)->privateId;
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
    res = Has<Node*, NodePtr>(sourcePtr->GetTargets(), targetPtr);
  }
  return res;
}

// Is edge exists in input graph.
template<class WeightInterface, typename WeightType>  bool Graph<WeightInterface, WeightType>::IsEgdeExists(ObjectId source, ObjectId target, bool onlyInSourceGraph) const
{
  bool res = false;
  NodePtr sourcePtr, targetPtr;
  if (IsValidNodeId(source, sourcePtr) && IsValidNodeId(target, targetPtr))
  {
    for (int i = 0; i < m_edges.size(); i++)
    {
      auto& edge = m_edges[i];
      if ((edge->source == sourcePtr && edge->target == targetPtr)
      || (!onlyInSourceGraph && !edge->direct && edge->source == targetPtr && edge->target == sourcePtr))
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
template<class WeightInterface, typename WeightType> WeightInterface* Graph<WeightInterface, WeightType>::MakeCopy(GraphCopyType type) const
{
    WeightInterface* res = NULL;
    switch (type)
    {
        case GCT_COPY:             res = MakeGraphCopy(); break;
        case GTC_MAKE_UNDIRECTED:  res = MakeGraphUndirected(); break;
        case GTC_INVERSE:          res = MakeGraphInverse(); break;
        case GTC_REMOVE_SELF_LOOP: res = MakeGraphRemoveSelfLoop(); break;
    }
    
    return res;
}

template<class WeightInterface, typename WeightType> IndexType Graph<WeightInterface, WeightType>::GetNextId()
{
    return ++m_autoIncIndex;
}

// Simple make copy
template<class WeightInterface, typename WeightType> Graph<WeightInterface, WeightType>* Graph<WeightInterface, WeightType>::MakeGraphCopy() const
{
    Graph<WeightInterface, WeightType>* res = new Graph<WeightInterface, WeightType>();
    
    CopyPropertiesTo(res);
    
    res->m_weightType = m_weightType;
    
    // Create all nodes.
    for (NodePtr node : m_nodes)
    {
        res->m_nodes.push_back(NodePtr(new Node(node->id, node->privateId, node->fake)));
    }
    
    // Add edges.
    for (EdgePtr edge : m_edges)
    {
        res->AddEdge(edge->id,
                     edge->source->privateId,
                     edge->target->privateId,
                     edge->direct,
                     edge->weight,
                     edge->privateId);
    }
    
    return res;
}

// Make current graph undirected.
template<class WeightInterface, typename WeightType> Graph<WeightInterface, WeightType>* Graph<WeightInterface, WeightType>::MakeGraphUndirected() const
{
    auto* res = MakeGraphCopy();
    
    if (res->m_hasDirected)
    {
        for (EdgePtr edge : res->m_edges)
        {
            if (edge->direct)
            {
                edge->direct = false;
                
                // Add another nodes.
                edge->target->AddToTargets(edge->source.get(), edge->privateId);
            }
        }
    }
    
    return res;
}

template<class WeightInterface, typename WeightType> typename Graph<WeightInterface, WeightType>::EdgePtr Graph<WeightInterface, WeightType>::AddEdge(const String& id, IndexType sourceId, IndexType targetId, bool direct, const WeightType& weight, IndexType privateId)
{
    EdgePtr res = FindEdge(sourceId, targetId);
    assert (!res);
    
    if (!res)
    {
        NodePtr sourceNode = FindNode(sourceId, m_nodes);
        NodePtr targetNode = FindNode(targetId, m_nodes);
        
        res = EdgePtr(new Edge(id, sourceNode, targetNode, direct,
                       weight, privateId));
        
        m_edges.push_back(res);
        
        sourceNode->AddToTargets(targetNode.get(), res->privateId);
        
        if (!direct)
        {
            targetNode->AddToTargets(sourceNode.get(), res->privateId);
        }
        
        m_hasDirected = m_hasDirected || direct;
        m_hasUndirected = m_hasUndirected || !direct;
    }
    
    return res;
}


template<class WeightInterface, typename WeightType> IGraph* Graph<WeightInterface, WeightType>::MakeBaseCopy(GraphCopyType type) const
{
    return MakeCopy(type);
}

// Is graph directed or not.
template<class WeightInterface, typename WeightType> bool Graph<WeightInterface, WeightType>::HasDirected() const
{
    return m_hasDirected;
}

template<class WeightInterface, typename WeightType> bool Graph<WeightInterface, WeightType>::HasUndirected() const
{
    return m_hasUndirected;
}

// Make current graph undirected.
template<class WeightInterface, typename WeightType> Graph<WeightInterface, WeightType>* Graph<WeightInterface, WeightType>::MakeGraphInverse() const
{
    Graph<WeightInterface, WeightType>* res = new Graph<WeightInterface, WeightType>();
    
    CopyPropertiesTo(res);
    
    // Create all nodes.
    for (NodePtr node : m_nodes)
    {
        res->m_nodes.push_back(NodePtr(new Node(node->id, node->privateId, node->fake)));
    }
    
    // Add edges.
    for (EdgePtr edge : m_edges)
    {
        res->AddEdge(edge->id,
                     edge->target->privateId,
                     edge->source->privateId,
                     edge->direct,
                     edge->weight,
                     edge->privateId);
    }
    
    return res;
}

template<class WeightInterface, typename WeightType> void Graph<WeightInterface, WeightType>::ProcessDFS(IEnumStrategy* pEnumStrategy, ObjectId startedNode) const
{
    BaseNodeEnumStrategy baseNodeEnumStrategy(pEnumStrategy);
    BaseEdgeEnumStrategy baseEdgeEnumStrategy(pEnumStrategy);
    
    IEnumStrategy* pStrategy = pEnumStrategy;
    if (pEnumStrategy->GetDefaultStrategy() == IEnumStrategy::DES_NODE)
    {
        pStrategy = &baseNodeEnumStrategy;
    }
    else if (pEnumStrategy->GetDefaultStrategy() == IEnumStrategy::DES_EDGE)
    {
        pStrategy = &baseEdgeEnumStrategy;
    }
    
    _ProcessDFS(pStrategy, FindNode(startedNode, m_nodes).get());
}

template<class WeightInterface, typename WeightType> void Graph<WeightInterface, WeightType>::_ProcessDFS(IEnumStrategy* pEnumStrategy, Node* node) const
{
    assert (node);
    
    pEnumStrategy->StartProcessNode(node->privateId);
    
    const std::vector<Node*>& nodes = node->GetTargets();
    for (auto iterator = nodes.cbegin(); iterator != nodes.cend(); iterator++)
    {
        if (pEnumStrategy->NeedProcessChild(node->privateId,
                                            (*iterator)->privateId,
                                            node->GetEdge(iterator - nodes.cbegin())))
        {
            _ProcessDFS(pEnumStrategy, *iterator);
        }
    }
    
    pEnumStrategy->FinishProcessNode(node->privateId);
}

template<class WeightInterface, typename WeightType> Graph<WeightInterface, WeightType>* Graph<WeightInterface, WeightType>::MakeGraphRemoveSelfLoop() const
{
    Graph<WeightInterface, WeightType>* res = new Graph<WeightInterface, WeightType>();
    
    CopyPropertiesTo(res);
    
    // Create all nodes.
    for (NodePtr node : m_nodes)
    {
        res->m_nodes.push_back(NodePtr(new Node(node->id, node->privateId, node->fake)));
    }
    
    // Add edges.
    for (EdgePtr edge : m_edges)
    {
        if (edge->source->privateId != edge->target->privateId)
        {
            res->AddEdge(edge->id,
                     edge->source->privateId,
                     edge->target->privateId,
                     edge->direct,
                     edge->weight,
                     edge->privateId);
        }
    }
    
    return res;
}


template<class WeightInterface, typename WeightType> void Graph<WeightInterface, WeightType>::RemoveEdge(ObjectId source, ObjectId target)
{
    EdgePtr edge = FindEdge(source, target);
    assert (edge);
    
    if (edge)
    {
        RemoveEdge(edge);
    }
}

template<class WeightInterface, typename WeightType> void Graph<WeightInterface, WeightType>::RemoveEdge(EdgePtr edge)
{
    if (!edge->direct)
    {
        edge->target->RemoveEdge(edge->privateId);
    }
    
    edge->source->RemoveEdge(edge->privateId);
    
    auto removeEdgePosition = std::find(m_edges.begin(), m_edges.end(), edge);
    m_edges.erase(removeEdgePosition);
}

// How many nodes are source for this node.
template<class WeightInterface, typename WeightType> IndexType Graph<WeightInterface, WeightType>::GetSourceNodesNumber(ObjectId source)
{
    IndexType res = 0;
    for (EdgePtr edge : m_edges)
    {
        if (edge->target->privateId == source ||
            (edge->source->privateId == source && !edge->direct))
        {
            res++;
        }
    }
    
    return res;
}


template<class WeightInterface, typename WeightType> void Graph<WeightInterface, WeightType>::CopyPropertiesTo(Graph<WeightInterface, WeightType>* pGraph) const
{
    pGraph->m_weightType  = m_weightType;
    pGraph->m_hasDirected = m_hasDirected;
    pGraph->m_hasUndirected = m_hasUndirected;
}

// Add edge
template<class WeightInterface, typename WeightType> bool Graph<WeightInterface, WeightType>::AddEdge(ObjectId source, ObjectId target, bool direct, const FloatWeightType& weight)
{
    auto indexId = GetNextId();
    
    EdgePtr edge = AddEdge(String().FromInt(indexId), source, target, direct, (WeightType)weight, indexId);
    
    return edge != nullptr;
}

// Add node
template<class WeightInterface, typename WeightType> ObjectId Graph<WeightInterface, WeightType>::AddNode(bool fake)
{
    auto idNode = GetNextId();
    return AddNode(String().FromInt(idNode), idNode, fake);
}

template<class WeightInterface, typename WeightType> ObjectId Graph<WeightInterface, WeightType>::AddNode(const String& idNode, IndexType privateId, bool fake)
{
    m_nodes.push_back(NodePtr(new Node(idNode, privateId, fake)));
    return m_nodes.back()->privateId;
}

template<class WeightInterface, typename WeightType> bool Graph<WeightInterface, WeightType>::IsFakeNode(ObjectId source)
{
    bool res = false;
    NodePtr nodePtr;
    if (IsValidNodeId(source, nodePtr) && nodePtr)
    {
        res = nodePtr->fake;
    }
    
    assert(nodePtr);
    
    return res;
}
