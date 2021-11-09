#include "BellmanFord.h"
#include "IAlgorithmFactory.h"
#include <stack>
#include <queue>
#include <unordered_map>
#include <climits>

/* BELLMAN FORD ALGORITHM*/

//	UTILITY FUNCTIONS DEFINITIONS
//	FUNCTION TO PUSH RESULTS ONTO VECTOR
template<class WeightTypeInterface, class WeightType> void BellmanFord<WeightTypeInterface, WeightType>::pushResult()
{
	for(auto order_vertex : previous_vertex_order)
	{
		std::vector<ObjectId> temp;
		ObjectId prev        = previous_vertex[order_vertex];
		ObjectId destination = order_vertex;

		temp.push_back(destination);
		temp.push_back(prev);
		
		while(prev != m_source)
		{
			prev = previous_vertex[prev];
			temp.push_back(prev);
		}
		std::reverse(temp.begin(), temp.end());
		m_path.push_back(temp);
	}
}

//	FUNCTION GETTING NUMBER OF RESULT AVAILABLE IN m_path VECTOR

template<class WeightTypeInterface, class WeightType> IndexType BellmanFord<WeightTypeInterface, WeightType>::GetResultCountUtility() const
{
	int res = 1;
	if (m_path.size() == 0)
	{
		return res;
	}
	else
	{
		for (const auto& subgraph : m_path)
			res += subgraph.size() + 1;
		if (res > 1)
		{
			// Remove last split
			res--;
		}
	}
	return res;
}


//	UTITLITY FUNCTION DEFINITION ENDS HERE

template<class WeightTypeInterface, class WeightType> BellmanFord<WeightTypeInterface, WeightType>::BellmanFord()
{
	m_source = 0;
	m_pGraph = NULL;
}

template<class WeightTypeInterface, class WeightType> bool BellmanFord<WeightTypeInterface, WeightType>::EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const
{
	bool res = false;
	switch (index)
	{
	case 0:
	{
		strncpy(outParamInfo->paramName, "start", sizeof(outParamInfo->paramName));
		outParamInfo->type = APT_NODE;
		res = true;
		break;
	}
	}
	return res;
}

template<class WeightTypeInterface, class WeightType> void BellmanFord<WeightTypeInterface, WeightType>::SetParameter(const AlgorithmParam* param)
{
	if (param)
	{
		if (strncmp(param->paramName, "start", sizeof(AlgorithmParam::paramName)) == 0)
		{
			m_source = param->data.id;
		}
	}
}

template<class WeightTypeInterface, class WeightType> void BellmanFord<WeightTypeInterface, WeightType>::SetGraph(const IGraph* pGraph)
{
  if (pGraph->IsMultiGraph())
  {
    std::shared_ptr<IMultiGraph> multiGraph = std::shared_ptr<IMultiGraph>(m_pAlgorithmFactory->CreateMultiGraph(pGraph));
    m_pGraph = dynamic_cast<const WeightTypeInterface*>(multiGraph->MakeBaseCopy(GTC_MULTI_TO_COMMON_GRAPH_MINIMAL_EDGES));
  }
  else
  {
    m_pGraph = dynamic_cast<const WeightTypeInterface*>(pGraph);
  }
}

//	CALCULATING PATH FROM SOURCE TO DESTINATION

template<class WeightTypeInterface, class WeightType> bool BellmanFord<WeightTypeInterface, WeightType>::Calculate()
{
	std::unordered_map<ObjectId, WeightType> dist;
	std::unordered_map<ObjectId, bool> vis;

	bool check_vis = false;		//	VARIABLE USED FOR CHECKING VISITED NODE IN EACH ITERATION
	bool res = false;	//	VARIABLE CHECKING IF CALCULATION IS POSSIBLE, RETURN VALUE
	bool change = false;	//	VARIABLE USED FOR CHECKING ANY CHANGE IN ITERATION

	std::queue<ObjectId> vertex_q;
	for(int i = 0; i < m_pGraph->GetNodesCount(); i++)
	{
		ObjectId u = m_pGraph->GetNode((IndexType)i);
		dist[u] = (WeightType)std::numeric_limits<WeightType>::max();
		vis[u] = false;
	}
	ObjectId u = m_pGraph->GetNode((IndexType)0);
	dist[m_source] = 0;

	for(IndexType i = 0; i < m_pGraph->GetNodesCount() - 1; i++)
	{
		vertex_q.push(m_source);
		while(!vertex_q.empty())
		{
			ObjectId u = vertex_q.front();
			vertex_q.pop();
			for(IndexType k = 0; k < m_pGraph->GetConnectedNodes(u); k++)
			{
				ObjectId v = m_pGraph->GetConnectedNode(u, k);
				if(vis[v] == check_vis)
				{
					vertex_q.push(v);
					vis[v] = !check_vis;
				}
				WeightType alt = m_pGraph->GetEdgeWeight(u, v);
				if(alt + dist[u] < dist[v])
				{
					previous_vertex_order.insert(v);
					previous_vertex[v] = u;
					dist[v] = alt + dist[u];
					change = true;
					res = true;
				}
			}
		}
		check_vis = !check_vis;
		if(change == false)		//	IF NOTHING CHANGED IN PREVIOUS ITERATION THEN BREAK
		{
			break;
		}
		else
		{
			change = false;
		}
	}

	pushResult();	//	FINALLY PUSH RESULTS INTO m_path
	return res;
}

template<class WeightTypeInterface, class WeightType> IndexType BellmanFord<WeightTypeInterface, WeightType>::GetResultCount() const
{
	int res = GetResultCountUtility();
	return res;
}

//	FUNCTIONS IMPLEMENTING RESULT

template<class WeightTypeInterface, class WeightType> AlgorithmResult BellmanFord<WeightTypeInterface, WeightType>::GetResult(IndexType index) const
{
	AlgorithmResult result;
	if (m_pGraph->GetEdgeWeightType() == WT_FLOAT && index == 0)
	{
		return AlgorithmResult((FloatWeightType)((float)m_path.size()));
	}
	if (index == 0)
	{
		return AlgorithmResult((IntWeightType)m_path.size());
	}
	index--;
	for (auto& subgraph : m_path)
	{
		if (index < subgraph.size())
		{
			result.type = ART_NODES_PATH;
			m_pGraph->GetNodeStrId(subgraph[index], result.strValue, sizeof(result.strValue));
			return result;
		}
		else if (index == subgraph.size())
		{
			result.type = ART_SPLIT_PATHS;
			return result;
		}
		index -= subgraph.size() + 1;
	}
	return AlgorithmResult();
}

template<class WeightTypeInterface, class WeightType> IndexType BellmanFord<WeightTypeInterface, WeightType>::GetHightlightNodesCount() const
{
	return GetResultCountUtility() - 1;
}

template<class WeightTypeInterface, class WeightType> ObjectId BellmanFord<WeightTypeInterface, WeightType>::GetHightlightNode(IndexType index) const
{
	for (auto& subgraph : m_path)
	{
		if (index < subgraph.size())
		{
			return subgraph[index];
		}
		index -= subgraph.size();
	}
	return 0;
}

template<class WeightTypeInterface, class WeightType> IndexType BellmanFord<WeightTypeInterface, WeightType>::GetHightlightEdgesCount() const
{
	IndexType res = 0;
	for (auto& path : m_path)
		res += path.size() - 1;

	return res;
}

template<class WeightTypeInterface, class WeightType> NodesEdge BellmanFord<WeightTypeInterface, WeightType>::GetHightlightEdge(IndexType index) const
{
	NodesEdge edge;
	for (auto& subgraph : m_path)
	{
		if (index < subgraph.size() - 1)
		{
			edge.source = subgraph[index];
			edge.target = subgraph[index + 1];
			edge.edgeId = m_pGraph->GetEdge(edge.source, edge.target);
			break;
		}
		index -= subgraph.size() - 1;
	}
	return edge;
}
