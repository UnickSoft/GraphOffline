

#include "Graph.h"
#include "pugixml.hpp"
#include <algorithm>
#include <math.h>
#include "Logger.h"
#include <unordered_set>

// Find Edge funtor.
class FindEdgeFunctor
{
public:
  FindEdgeFunctor(Graph::Node* sourceNode, Graph::Node* targetNode, const IndexType & index = 0)
  {
    this->sourceNode = sourceNode;
    this->targetNode = targetNode;
    this->index      = index;
  }
  bool operator()(Graph::EdgePtr edge)
  {
    if ((edge->source == sourceNode && edge->target == targetNode) ||
      (edge->source == targetNode && edge->target == sourceNode && !edge->direct))
    {
        // Count indexes.
        if (index > 0)
          index--;
        else
          return true;
    }
    
    return false;
  }
  Graph::Node* sourceNode;
  Graph::Node* targetNode;
  IndexType    index;
};


// Base Enum to exclude infinity loop using Nodes.
class BaseNodeEnumStrategy : public IEnumStrategy
{
public:
    BaseNodeEnumStrategy (IEnumStrategy* pUserStrategy) : m_pUserStrategy(pUserStrategy) {;}
    
    virtual void StartProcessNode(ObjectId nodeId) override
    {
        processedNode.insert(nodeId);
        m_pUserStrategy->StartProcessNode(nodeId);
    }
    
    virtual bool NeedProcessChild(ObjectId nodeId, ObjectId childId, ObjectId edge) override
    {
        return m_pUserStrategy->NeedProcessChild(nodeId, childId, edge) &&
            processedNode.find(childId) == processedNode.end();
    }
    
    virtual void FinishProcessNode(ObjectId nodeId) override
    {
        m_pUserStrategy->FinishProcessNode(nodeId);
    }
    
    // Default Strategy.
    virtual DefaultEnumStrategy GetDefaultStrategy() override
    {
        return DES_NONE;
    }
    
    // We started process this edge.
    void StartProcessEdge(ObjectId edgeId) override {}
    // We finish process this edge.
    void FinishProcessEdge(ObjectId edgeId) override {}
    
protected:
    IEnumStrategy* m_pUserStrategy;
    std::unordered_set<ObjectId> processedNode;
};

// Base Enum to exclude infinity loop using Edges.
class BaseEdgeEnumStrategy : public IEnumStrategy
{
public:
    BaseEdgeEnumStrategy (IEnumStrategy* pUserStrategy) : m_pUserStrategy(pUserStrategy) {;}
    
    virtual void StartProcessNode(ObjectId nodeId) override
    {
        m_pUserStrategy->StartProcessNode(nodeId);
    }
    
    virtual bool NeedProcessChild(ObjectId nodeId, ObjectId childId, ObjectId edge) override
    {
        return m_pUserStrategy->NeedProcessChild(nodeId, childId, edge) &&
        (processedEdge.find(edge) == processedEdge.end());
    }
    
    virtual void FinishProcessNode(ObjectId nodeId) override
    {
        m_pUserStrategy->FinishProcessNode(nodeId);
    }
    
    // Default Strategy.
    virtual DefaultEnumStrategy GetDefaultStrategy() override
    {
        return DES_NONE;
    }
    
    // We started process this edge.
    void StartProcessEdge(ObjectId edgeId) override
    {
        processedEdge.insert(edgeId);
    }
    
    // We finish process this edge.
    void FinishProcessEdge(ObjectId edgeId) override
    {
        processedEdge.erase(edgeId);
    }
    
protected:
    IEnumStrategy* m_pUserStrategy;
    std::unordered_set<ObjectId> processedEdge;
};

// 0 is invalid value. 1000 - because it is not the same as index.
IndexType Graph::m_autoIncIndex = 1000;

Graph::Graph(void)
{
    m_weightType    = WT_INT;
    m_hasDirected   = false;
    m_hasUndirected = false;
    m_isMultigraph  = false;
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


void Graph::Clear()
{
  m_nodes.clear();
  m_edges.clear();
  m_idToNode.clear();
}

Graph::NodePtr Graph::FindNode(const String& id) const
{
  return FindObject(id, m_nodes);
}

Graph* Graph::CreateGraph() const
{
    return new Graph();
}

template <typename T> T Graph::FindObject(const String& id, const std::vector<T>& nodes) const
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

template <typename T> T Graph::FindObject(ObjectId objectId, const std::vector<T>& nodes) const
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

template <typename T> T Graph::FindObject(ObjectId objectId, const std::unordered_map<ObjectId, T>& searchMap) const
{
    auto objectItr = searchMap.find(objectId);

    return objectItr != searchMap.end() ? objectItr->second : T(nullptr);
}

// Get Nodes count.
IndexType Graph::GetNodesCount() const
{
  return IndexType(m_nodes.size());
}

ObjectId Graph::GetNode(IndexType index) const
{
  return m_nodes[index].get()->privateId;
}

ObjectId Graph::GetNode(const char* nodeId) const
{
    NodePtr res = FindNode(String().FromLocale(nodeId));
    return res ? res.get()->privateId : 0;
}

// Get Edges count.
IndexType Graph::GetEdgesCount() const
{
  return IndexType(m_edges.size());
}

// Get connected graph count.
IndexType Graph::GetConnectedNodes(ObjectId source) const
{
    IndexType res = 0;
    NodePtr nodePtr;
    if (IsValidNodeId(source, nodePtr) && nodePtr)
    {
        res = IndexType(nodePtr->GetTargets().size());
    }
    
    assert(nodePtr);
    
    return res;
}

// Get connected graph for this graph.
ObjectId Graph::GetConnectedNode(ObjectId source, IndexType index) const
{
    ObjectId res = 0;
    NodePtr sourcePtr;
    if (IsValidNodeId(source, sourcePtr))
    {
        res = sourcePtr->GetTargets().at(index)->privateId;
    }
    return res;
}

IndexType Graph::GetConnectedNodeIndex(ObjectId source, IndexType index) const
{
    IndexType res = -1;
    NodePtr sourcePtr;
    if (IsValidNodeId(source, sourcePtr))
    {
        res = sourcePtr->GetTargets().at(index)->index;
    }
    assert(res != -1);
    return res;
}

// Is edge exists.
bool Graph::AreNodesConnected(ObjectId source, ObjectId target) const
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
bool Graph::IsEgdeExists(ObjectId source, ObjectId target, bool onlyInSourceGraph) const
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
Graph::WeightType* Graph::GetEdgeWeight(ObjectId source, ObjectId target, const IndexType & index) const
{
    WeightType* res = nullptr;
    EdgePtr edge = FindEdge(source, target);
    assert(edge);

    if (edge)
    {
        res = (WeightType*)&edge->weight;
    }
    else
    {
        LOG_WARNING("Cannot find edge from " << source << " to " << target);
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

bool Graph::GetEdgeStrId(ObjectId edge, char* outBuffer, IndexType bufferSize) const
{
    bool res = false;
    EdgePtr edgePtr;
    if (IsValidEdgeId(edge, edgePtr))
    {
        res = edgePtr->id.PrintLocale(outBuffer, bufferSize) > 0;
    }
    return res;
}

bool Graph::IsValidNodeId(ObjectId id, NodePtr& ptr) const
{
    ptr = FindObject(id, m_idToNode);
    return ptr != nullptr;
}

bool Graph::IsValidEdgeId(ObjectId id, EdgePtr& ptr) const
{
    ptr = FindObject(id, m_edges);
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

Graph::EdgePtr Graph::FindEdge(ObjectId source, ObjectId target, const IndexType & index) const
{
    EdgePtr res;
    NodePtr sourcePtr, targetPtr;
    if (IsValidNodeId(source, sourcePtr) && IsValidNodeId(target, targetPtr))
    {
        auto edgeIterator =
        std::find_if(m_edges.begin(), m_edges.end(), FindEdgeFunctor(sourcePtr.get(), targetPtr.get(), index));
        
        if (edgeIterator != m_edges.end())
        {
            res = *edgeIterator;
        }
    }
    
    return res;
}

bool Graph::IsDouble(double value)
{
    return fabs(value - floor(value)) > 1E-5;
}

EdgeWeightType Graph::GetEdgeWeightType() const
{
    return m_weightType;
}


// Create copy of graph.
Graph* Graph::MakeGraphCopy(GraphCopyType type, const std::function<Graph*()> & createFunction) const
{
    Graph* res = NULL;
    switch (type)
    {
        case GCT_COPY:             res = MakeGraphCopy(createFunction); break;
        case GTC_MAKE_UNDIRECTED:  res = MakeGraphUndirected(createFunction); break;
        case GTC_INVERSE:          res = MakeGraphInverse(createFunction); break;
        case GTC_REMOVE_SELF_LOOP: res = MakeGraphRemoveSelfLoop(createFunction); break;
        case GTC_REMOVE_NEGATIVE:  res = MakeGraphRemoveNegative(createFunction); break;
    }

    if (res)
    {
      res->ForceUpdateStates();
    }
    
    return res;
}

ObjectId Graph::GetEdge(IndexType index) const
{
    return m_edges[index].get()->privateId;
}

Graph::NodePair Graph::GetEdgeData(IndexType index) const
{
  NodePair res;
  auto edge = m_edges[index];
  res.source = edge->source->privateId;
  res.target = edge->target->privateId;
  return res;
}

// Edge connected nodes
Graph::NodePair Graph::GetEdgeData(ObjectId objectId) const
{
    Graph::NodePair res;
    for (const auto & edge : m_edges)
    {
        if (edge->privateId == objectId)
        {
            res.source = edge->source->privateId;
            res.target = edge->target->privateId;
            break;
        }          
    }
    return res;
}

IndexType Graph::GetNextId()
{
    return ++m_autoIncIndex;
}

// Simple make copy
Graph* Graph::MakeGraphCopy(const std::function<Graph*()> & createFunction) const
{
    Graph* res = createFunction ? createFunction() : CreateGraph();
    
    CopyPropertiesTo(res);
    
    res->m_weightType = m_weightType;
    
    // Create all nodes.
    for (NodePtr node : m_nodes)
    {
        res->m_nodes.push_back(NodePtr(new Node(node->id, node->privateId, node->fake, node->index)));
        res->m_idToNode[node->privateId] = res->m_nodes.back();
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
Graph* Graph::MakeGraphUndirected(const std::function<Graph*()> & createFunction) const
{
    auto* res = MakeGraphCopy(createFunction);
    
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

Graph::EdgePtr Graph::AddEdge(const String& id, IndexType sourceId, IndexType targetId, bool direct, const WeightType& weight, IndexType privateId)
{
    EdgePtr checkEdge = FindEdge(sourceId, targetId);

    if (checkEdge && !m_isMultigraph)
    {
      m_isMultigraph = true;
    }

    NodePtr sourceNode = FindObject(sourceId, m_idToNode);
    NodePtr targetNode = FindObject(targetId, m_idToNode);
    
    EdgePtr res = EdgePtr(new Edge(id, sourceNode, targetNode, direct,
                   weight, privateId));
    
    m_edges.push_back(res);
    
    sourceNode->AddToTargets(targetNode.get(), res->privateId);
    
    if (!direct)
    {
        targetNode->AddToTargets(sourceNode.get(), res->privateId);
    }
    
    m_hasDirected = m_hasDirected || direct;
    m_hasUndirected = m_hasUndirected || !direct;
    
    return res;
}

IGraph* Graph::MakeBaseCopy(GraphCopyType type) const
{
    return MakeGraphCopy(type);
}

// Is graph directed or not.
bool Graph::HasDirected() const
{
    return m_hasDirected;
}

bool Graph::HasUndirected() const
{
    return m_hasUndirected;
}

// Make current graph undirected.
Graph* Graph::MakeGraphInverse(const std::function<Graph*()> & createFunction) const
{
    Graph* res = createFunction ? createFunction() : CreateGraph();
    
    CopyPropertiesTo(res);
    
    // Create all nodes.
    for (NodePtr node : m_nodes)
    {
        res->m_nodes.push_back(NodePtr(new Node(node->id, node->privateId, node->fake, node->index)));
        res->m_idToNode[node->privateId] = res->m_nodes.back();
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

void Graph::ProcessDFS(IEnumStrategy* pEnumStrategy, ObjectId startedNode) const
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
    
    LOG_INFO("DFS start");
    _ProcessDFS(pStrategy, FindObject(startedNode, m_idToNode).get());
    LOG_INFO("DFS finish");
}

void Graph::_ProcessDFS(IEnumStrategy* pEnumStrategy, Node* node) const
{
    assert (node);
    
    pEnumStrategy->StartProcessNode(node->privateId);
    
    LOG_INFO("DFS: start " << node->id.Locale().Data());
    
    const std::vector<Node*>& nodes = node->GetTargets();
    for (auto iterator = nodes.cbegin(); iterator != nodes.cend(); iterator++)
    {
        auto edge = node->GetEdge(IndexType(iterator - nodes.cbegin()));
        if (pEnumStrategy->NeedProcessChild(node->privateId,
                                            (*iterator)->privateId, edge))
        {
            pEnumStrategy->StartProcessEdge(edge);
            _ProcessDFS(pEnumStrategy, *iterator);
            pEnumStrategy->FinishProcessEdge(edge);
        }
    }
    
    LOG_INFO("DFS: finish " << node->id.Locale().Data());
    pEnumStrategy->FinishProcessNode(node->privateId);
}

Graph* Graph::MakeGraphRemoveSelfLoop(const std::function<Graph*()> & createFunction) const
{
    Graph* res = createFunction ? createFunction() : CreateGraph();
    
    CopyPropertiesTo(res);
    
    // Create all nodes.
    for (NodePtr node : m_nodes)
    {
        res->m_nodes.push_back(NodePtr(new Node(node->id, node->privateId, node->fake, node->index)));
        res->m_idToNode[node->privateId] = res->m_nodes.back();
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

Graph* Graph::MakeGraphRemoveNegative(const std::function<Graph*()> & createFunction) const
{
    LOG_ERROR("Cannot remove negative edges. Weight type is unknown");
    
    return nullptr;
}


void Graph::RemoveEdge(ObjectId source, ObjectId target)
{
    EdgePtr edge = FindEdge(source, target);
    assert (edge);
    
    if (edge)
    {
        RemoveEdge(edge);
        ForceUpdateStates();
    }
}

void Graph::RemoveEdge(EdgePtr edge)
{
    if (!edge->direct)
    {
        edge->target->RemoveEdge(edge->privateId);
    }
    
    edge->source->RemoveEdge(edge->privateId);
    
    auto removeEdgePosition = std::find(m_edges.begin(), m_edges.end(), edge);
    m_edges.erase(removeEdgePosition);
}

Graph::EdgePtr Graph::GetEdgeById(ObjectId edgeId)
{
    return FindObject(edgeId, m_edges);
}

const Graph::EdgePtr Graph::GetEdgeById(ObjectId edgeId) const
{
    return FindObject(edgeId, m_edges);
}

// How many nodes are source for this node.
IndexType Graph::GetSourceNodesNumber(ObjectId source)
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

void Graph::CopyPropertiesTo(Graph* pGraph) const
{
    pGraph->m_weightType    = m_weightType;
    pGraph->m_hasDirected   = m_hasDirected;
    pGraph->m_hasUndirected = m_hasUndirected;
    pGraph->m_isMultigraph  = m_isMultigraph;
}

void Graph::ForceUpdateStates()
{
  m_hasDirected   = false;
  m_hasUndirected = false;
  m_isMultigraph  = false;

  struct EdgeData
  {
    ObjectId node1;
    ObjectId node2;
    bool     directed;
    bool operator==(const EdgeData & data) const
    {
      return node1 == data.node1 && node2 == data.node2 && directed == data.directed;
    }
  };

  struct Hash {
    size_t operator() (const EdgeData &edge) const {
      return std::hash<int>{}(edge.node1 + edge.node2 + (edge.directed ? 1 : 0));
    }
  };

  std::unordered_set<EdgeData, Hash> processed;

  for (const auto & edge : m_edges)
  {
    m_hasDirected = m_hasDirected || edge->direct;
    m_hasUndirected = m_hasUndirected || !edge->direct;

    EdgeData data{ edge->source->privateId,  edge->target->privateId, edge->direct };
    if (processed.count(data) > 0)
      m_isMultigraph = true;

    if (!data.directed)
    {
      EdgeData dataReverse{ edge->target->privateId, edge->source->privateId, edge->direct };

      if (processed.count(dataReverse) > 0)
        m_isMultigraph = true;

      EdgeData dataDirected1{ edge->target->privateId, edge->source->privateId, true };
      EdgeData dataDirected2{ edge->source->privateId, edge->target->privateId, true };

      if (processed.count(dataDirected1) > 0)
        m_isMultigraph = true;

      if (processed.count(dataDirected2) > 0)
        m_isMultigraph = true;
    }

    processed.insert(data);
  }
}

// Add edge
bool Graph::AddEdge(ObjectId source, ObjectId target, bool direct, const FloatWeightType& weight)
{
    auto indexId = GetNextId();
    
    EdgePtr edge = AddEdge(String().FromInt(indexId), source, target, direct, (WeightType)weight, indexId);
    
    return edge != nullptr;
}

// Add node
ObjectId Graph::AddNode(bool fake)
{
    auto idNode = GetNextId();
    return AddNode(String().FromInt(idNode), idNode, fake);
}

ObjectId Graph::AddNode(const String& idNode, IndexType privateId, bool fake)
{
    m_nodes.push_back(NodePtr(new Node(idNode, privateId, fake, m_nodes.size())));
    m_idToNode[m_nodes.back()->privateId] = m_nodes.back();
    
    return m_nodes.back()->privateId;
}

bool Graph::IsFakeNode(ObjectId source)
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

const char* Graph::PrintGraph() const
{
    static std::string report;
    
    report = "Graph has nodes " + std::to_string(GetNodesCount()) + " and edges " + std::to_string(GetEdgesCount());

    report += "\nNodes:\n";
    for (auto node : m_nodes)
    {
        report += std::string("Public id: ") + node->id.Locale().Data() + " " + "private id: " + std::to_string(node->privateId) + (node->fake ? " is fake\n" : "\n");
    }
    
    report += "Edges:\n";
    for (auto edge : m_edges)
    {
        report += std::string("Public id: ") + edge->id.Locale().Data() + " " + "private id: " + std::to_string(edge->privateId) + "\n";
    }
    
    return report.c_str();
}

// Has multi graph
bool Graph::IsMultiGraph() const
{
  return m_isMultigraph;
}

ObjectId Graph::GetEdge(ObjectId source, ObjectId target) const
{
    auto edge = FindEdge(source, target);
    
    if (edge)
    {
        return edge->privateId;
    }
    
    return 0;
}

void Graph::RemoveNode(ObjectId source)
{
  NodePtr nodePtr;
  if (IsValidNodeId(source, nodePtr))
  {
    EdgePtrVector edgesForRemove;

    m_edges.erase(std::remove_if(
      m_edges.begin(),
      m_edges.end(),
      [nodePtr](EdgePtr & edge) 
      {
        return (edge->source == nodePtr || edge->target == nodePtr); 
      }
    ), m_edges.end());

    m_nodes.erase(std::find(m_nodes.begin(), m_nodes.end(), nodePtr));
    m_idToNode.erase(nodePtr->privateId);
  }
}
