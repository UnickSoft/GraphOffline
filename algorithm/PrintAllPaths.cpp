#include "PrintAllPaths.h"
#include <stack>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <cstring>
#include <iterator>
#include <string>

#include "IAlgorithmFactory.h"

//	UTILITY FUNCTIONS DEFINITIONS
//	FUNCTION TO CHECK IF THE NODE IS NOT VISITED

static int isNotVisited(ObjectId x, const std::vector<ObjectId> & path)
{
	int size = path.size();
	for (int i = 0; i < size; i++)
		if (path[i] == x)
			return 0;
	return 1;
}
// Change


//	FUNCTION TO PUSH RESULTS ONTO VECTOR
void PrintAllPaths::pushResult(const std::vector<ObjectId> &path)
{
	m_path.push_back(path);
	for (int i = 0; i < path.size(); i ++)
	{
		if (i % 2 == 0)
		{
			m_nodes.insert(path.at(i));
		}
		else
		{
			m_edges.insert(path.at(i));
		}
	}
}

//	FUNCTION GETTING NUMBER OF RESULT AVAILABLE IN m_path VECTOR
IndexType PrintAllPaths::GetResultCountUtility() const
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

PrintAllPaths::PrintAllPaths()
{
	m_source = 0;
	m_target = 0;
	m_pGraph = NULL;
}

bool PrintAllPaths::EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const
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
		case 1:
		{
			strncpy(outParamInfo->paramName, "finish", sizeof(outParamInfo->paramName));
			outParamInfo->type = APT_NODE;
			res = true;
			break;
		}
	}
	return res;
}

void PrintAllPaths::SetParameter(const AlgorithmParam* param)
{
	if (param)
	{
		if (strncmp(param->paramName, "start", sizeof(AlgorithmParam::paramName)) == 0)
		{
			m_source = param->data.id;
		}
		if (strncmp(param->paramName, "finish", sizeof(AlgorithmParam::paramName)) == 0)
		{
			m_target = param->data.id;
		}
	}
}

//	CALCULATING PATH FROM SOURCE TO DESTINATION

bool PrintAllPaths::Calculate()
{
	return m_pGraph->IsMultiGraph() ?
	 	CalculateMultiGraph() :
		CalculateNormalGraph();
}

IndexType PrintAllPaths::GetResultCount() const
{
	int res = GetResultCountUtility();
	return res;
}

AlgorithmResult PrintAllPaths::GetResult(IndexType index) const
{
	AlgorithmResult result;
	if (index == 0)
	{
		return AlgorithmResult((IntWeightType)m_path.size());
	}
	index--;
	for (auto& subgraph : m_path)
	{
		if (index < subgraph.size())
		{
			if (index % 2 == 0)
			{
				result.type = ART_NODES_PATH;
				m_pGraph->GetNodeStrId(subgraph[index], result.strValue, sizeof(result.strValue));
				return result;
			}
			else
			{
				result.type = ART_EDGES_PATH;
				m_pGraph->GetEdgeStrId(subgraph[index], result.strValue, sizeof(result.strValue));
				if (result.strValue[0] == 0)
				{
					std::string s = std::to_string(subgraph[index]);
					char const *str = s.c_str();
					strncpy(result.strValue, str, s.length() + 1);
				}
				return result;
			}
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

IndexType PrintAllPaths::GetHightlightNodesCount() const
{
	return m_nodes.size();
}

ObjectId PrintAllPaths::GetHightlightNode(IndexType index) const
{
	if (index > m_nodes.size())
	{
		return -1;
	}

	return *std::next(m_nodes.begin(), index);
}

IndexType PrintAllPaths::GetHightlightEdgesCount() const
{
	return m_edges.size();
}

NodesEdge PrintAllPaths::GetHightlightEdge(IndexType index) const
{
	if (index > m_edges.size())
	{
		return NodesEdge();
	}

	ObjectId edge_id = *std::next(m_edges.begin(), index);
	auto edge_nodes = m_pGraph->GetEdgeData(edge_id);

	NodesEdge edge;
	edge.source = edge_nodes.source;
	edge.target = edge_nodes.target;
	edge.edgeId = edge_id;

	return edge;
}

bool PrintAllPaths::CalculateNormalGraph()
{
	std::queue<std::vector<ObjectId>> q;
	std::vector<ObjectId> path;
	path.push_back(m_source);
	q.push(path);
	while (!q.empty())
	{
		path = q.front();
		q.pop();
		ObjectId last = path[path.size() - 1];
		if (last == m_target)		// IF WE REACH DESTINATION THEN WE PUSH THE RESULT
		{
			pushResult(path);
		}
		for (IndexType i = 0; i < m_pGraph->GetConnectedNodes(last); i++)	// LOOP THROUGH CONNECTED NODES OF LAST VERTEX
		{
			ObjectId v = m_pGraph->GetConnectedNode(last, i);		//	GET EACH OF THE CONNECTED NODES
			if (isNotVisited(v, path))
			{
				std::vector<ObjectId> new_path(path);		// IF NODE IS NOT VISISTED THEN WE PUSH THE VERTEX ONTO NEW RESULT FOR NEXT ITERATIONS
				auto edge = m_pGraph->GetEdge(last, v);
				new_path.push_back(edge);
				new_path.push_back(v);
				q.push(new_path);
			}
		}
	}
	return true;
}

bool PrintAllPaths::CalculateMultiGraph()
{
	const IMultiGraph* multiGraph = m_pAlgorithmFactory->CreateMultiGraph(m_pGraph);

	std::queue<std::vector<ObjectId>> q;
	std::vector<ObjectId> path;
	path.push_back(m_source);
	q.push(path);
	while (!q.empty())
	{
		path = q.front();
		q.pop();
		ObjectId last = path[path.size() - 1];
		if (last == m_target)		// IF WE REACH DESTINATION THEN WE PUSH THE RESULT
		{
			pushResult(path);
		}
		for (IndexType i = 0; i < multiGraph->GetConnectedNodes(last); i++)	// LOOP THROUGH CONNECTED NODES OF LAST VERTEX
		{
			ObjectId v = multiGraph->GetConnectedNode(last, i);		//	GET EACH OF THE CONNECTED NODES
			if (isNotVisited(v, path))
			{
				auto edges_number = multiGraph->GetEdgesNumber(last, v);
				for (IndexType e = 0; e < edges_number; e++)
				{
					ObjectId edge = multiGraph->GetEdge(last, v, e);
					std::vector<ObjectId> new_path(path);
					new_path.push_back(edge);
					new_path.push_back(v);
					q.push(new_path);
				}
			}
		}
	}
	return true;
}
