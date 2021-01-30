//
//  ConnectedСomponent.cpp
//  Graphoffline
//
//  Created by Олег on 27.12.15.
//
//

#include "IsomorphismCheck.h"
#include <unordered_set>
#include <queue>
#include <stack>
#include <string.h>
#include "CBioInfCpp.h"

static const char* g_firstGraph  = "graph1";
static const char* g_secondGraph = "graph2";
static const char* g_checkSubGraph = "searchSubgraphs";

static std::unordered_set<ObjectId> GetNodesSet(ObjectId* ids)
{
    std::unordered_set<ObjectId> res;
    if (ids == nullptr)
      return res;

    int index = 0;
    while (ids[index] != std::numeric_limits<ObjectId>::max())
    {
        res.insert(ids[index]);
        index++;
    }
    return res;
}

IsomorphismCheck::IsomorphismCheck()
{
    m_pGraph = nullptr;
    m_searchSubgraphs = false;
    m_isIsomorph = false;
}

IsomorphismCheck::~IsomorphismCheck()
{
    
}

// Enum parameters
bool IsomorphismCheck::EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const
{
    if (index == 0)
    {
        strncpy(outParamInfo->paramName, g_firstGraph, sizeof(outParamInfo->paramName));
        outParamInfo->type = APT_EDGE_LIST;
        return true;
    }
    else if (index == 1)
    {
      strncpy(outParamInfo->paramName, g_secondGraph, sizeof(outParamInfo->paramName));
      outParamInfo->type = APT_EDGE_LIST;
      return true;
    }
    else if (index == 2)
    {
      strncpy(outParamInfo->paramName, g_checkSubGraph, sizeof(outParamInfo->paramName));
      outParamInfo->type = APT_FLAG;
      return true;
    }
    
    return false;
}

// Set parameter to algorithm.
void IsomorphismCheck::SetParameter(const AlgorithmParam* outParamInfo)
{
    if (outParamInfo && strncmp(outParamInfo->paramName, g_firstGraph, sizeof(AlgorithmParam::paramName)) == 0)
    {
        m_firstGraphEdges = GetNodesSet(outParamInfo->data.ids);
    }
    else if (outParamInfo && strncmp(outParamInfo->paramName, g_secondGraph, sizeof(AlgorithmParam::paramName)) == 0)
    {
        m_secondGraphEdges = GetNodesSet(outParamInfo->data.ids);
    }
    else if (outParamInfo && strncmp(outParamInfo->paramName, g_checkSubGraph, sizeof(AlgorithmParam::paramName)) == 0)
    {
        m_searchSubgraphs = outParamInfo->data.bFlag;
    }
}

// Calculate algorithm.
bool IsomorphismCheck::Calculate()
{
    auto graph1 = GetSubGraph(m_firstGraphEdges);
    auto graph2 = GetSubGraph(m_secondGraphEdges);

    if (graph1 == nullptr || graph2 == nullptr)
      return false;

    std::vector<int> graph1AsPair;
    std::vector<int> graph2AsPair;
    std::set<std::vector <int>> subGraphs;

    GetGraphAsPairOfNodes(graph1, graph1AsPair);
    GetGraphAsPairOfNodes(graph2, graph2AsPair);

    bool fullDirected1   = graph1->HasDirected() && !graph1->HasUndirected();
    bool fullUndirected1 = !graph1->HasDirected() && graph1->HasUndirected();

    bool fullDirected2   = graph2->HasDirected() && !graph2->HasUndirected();
    bool fullUndirected2 = !graph2->HasDirected() && graph2->HasUndirected();

    if (graph2AsPair.size() != graph1AsPair.size() && !m_searchSubgraphs)
    {
      m_isIsomorph = false;
      return true;
    }
    
    if ((fullDirected1 || fullUndirected1) && (fullDirected2 || fullUndirected2) &&
        (fullDirected1 == fullDirected1 && fullUndirected1 == fullUndirected2))
    {
      int res = SubGraphsInscribed(graph2AsPair, graph1AsPair, subGraphs, fullDirected1, 0, 1, m_searchSubgraphs ? graph2->GetNodesCount() : 1);

      if (m_searchSubgraphs)
      {
        m_isIsomorph = res > 0 ? subGraphs.size() > 0 : 0;
        for (auto & oneSubgraph : subGraphs)
        {
          std::vector<ObjectId> subgraph;
          for (int i = 0; i < oneSubgraph.size(); i+=2)
          {
            auto index1 = oneSubgraph[i];
            auto index2 = oneSubgraph[i + 1];

            subgraph.push_back(graph2->GetEdge(graph2->GetNode(index1), graph2->GetNode(index2)));
            m_edgesInSubGraph.insert(subgraph.back());
          }
          m_subGraphs.push_back(std::move(subgraph));
        }
      }
      else
      {
        m_isIsomorph = res > 0 && subGraphs.begin()->size() == graph2AsPair.size();
      }
    }

    return true;
}

GraphPtr IsomorphismCheck::GetSubGraph(const std::unordered_set<ObjectId> & nodesSet)
{
  auto graphWithoutSelfLoops = GraphPtr(m_pGraph->MakeBaseCopy(GTC_REMOVE_SELF_LOOP));
  auto graph                 = GraphPtr(graphWithoutSelfLoops->MakeBaseCopy(GCT_COPY));

  std::vector<IGraph::NodePair> edgeToRemove;
  for (IndexType i = 0; i < graph->GetEdgesCount(); i++)
  {
    auto edgeId = graph->GetEdge(i);
    if (nodesSet.count(edgeId) == 0)
    {
      edgeToRemove.push_back(graph->GetEdgeData(i));
    }
  }

  for (auto & edge : edgeToRemove)
  {
    graph->RemoveEdge(edge.source, edge.target);
  }

  // Remove isolated nodes.
  std::vector<ObjectId> nodesToRemove;
  for (int i = 0; i < graph->GetNodesCount(); i++)
  {
    auto nodeId = graph->GetNode(i);
    if (graph->GetSourceNodesNumber(nodeId) == 0 && graph->GetConnectedNodes(nodeId) == 0)
      nodesToRemove.push_back(nodeId);
  }

  for (auto & node : nodesToRemove)
  {
    graph->RemoveNode(node);
  }

  return graph;
}

void IsomorphismCheck::GetGraphAsPairOfNodes(GraphPtr graph, std::vector<int> & result)
{
  std::unordered_map<ObjectId, int> nodesList;
  for (int i = 0; i < graph->GetNodesCount(); i++)
    nodesList[graph->GetNode(i)] = i;

  for (IndexType i = 0; i < graph->GetEdgesCount(); i++)
  {
    auto connectedData = graph->GetEdgeData(i);
    result.push_back(nodesList[connectedData.source]);
    result.push_back(nodesList[connectedData.target]);
  }
}

// Get result count.
IndexType IsomorphismCheck::GetResultCount() const
{
    if (!m_isIsomorph)
    {
      return 1;
    }
    else
    {
      int res = 1;
      for (const auto & subgraph : m_subGraphs)
        res += subgraph.size() + 1;
      
      if (res > 1)
      {
        // Remove last split
        res--;
      }

      return res;
    }
}

// Get result.
AlgorithmResult IsomorphismCheck::GetResult(IndexType index) const
{
  if (!m_searchSubgraphs)
  {
    return AlgorithmResult(m_isIsomorph);
  }
  else
  {
    if (index == 0)
      return AlgorithmResult((IntWeightType)m_subGraphs.size());

    index--;
    for (auto & subgraph : m_subGraphs)
    {
      if (index < subgraph.size())
      {
        AlgorithmResult result;
        result.type = ART_EDGES_PATH;
        m_pGraph->GetEdgeStrId(subgraph[index], result.strValue,
          sizeof(result.strValue));

        return result;
      }
      else if (index == subgraph.size())
      {
        AlgorithmResult result;
        result.type = ART_SPLIT_PATHS;

        return result;
      }
      
      index -= subgraph.size() + 1;
    }

    return AlgorithmResult();
  }
}

bool IsomorphismCheck::GetEdgeProperty(const NodesEdge& object, IndexType properyIndex,
  IndexType resultEdgeIndex, AlgorithmResult* param) const
{
  if (properyIndex == 0)
  {
    if (m_edgesInSubGraph.count(object.edgeId) == 0)
      return false;

    int groupIndex = 0;
    for (auto & subgraph : m_subGraphs)
    {
      if (resultEdgeIndex < subgraph.size())
      {
        param->nValue = groupIndex;
        param->type = ART_INT;
        return true;
      }

      resultEdgeIndex -= subgraph.size();
      groupIndex++;
    }

    return false;
  }

  return false;
}

const char* IsomorphismCheck::GetEdgePropertyName(IndexType index) const
{
    if (index == 0)
    {
        return "subgraph_index";
    }

    return nullptr;
}

// Hightlight edges count.
IndexType IsomorphismCheck::GetHightlightEdgesCount() const
{
  IndexType res = 0;

  for (auto & subgraph : m_subGraphs)
  {
    res += subgraph.size();
  }

  return res;
}

// Hightlight edge.
NodesEdge IsomorphismCheck::GetHightlightEdge(IndexType index) const
{
  NodesEdge res;
  for (auto & subgraph : m_subGraphs)
  {
      if (index < subgraph.size())
      {
        auto edge     = subgraph.at(index);
        auto edgeData = m_pGraph->GetEdgeData(edge);
        res.edgeId    = edge;
        res.source    = edgeData.source;
        res.target    = edgeData.target;
      }

      index -= subgraph.size();
  }

  return res;
}



