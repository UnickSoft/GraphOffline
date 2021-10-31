#include "PrintAllPaths.h"
#include <stack>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <cstring>

//	UTILITY FUNCTIONS DEFINITIONS

//	FUNCTION TO CHECK IF THE NODE IS NOT VISITED

int PrintAllPaths::isNotVisited(ObjectId x, std::vector<ObjectId>& path)
{
	int size = path.size();
	for (int i = 0; i < size; i++)
		if (path[i] == x)
			return 0;
	return 1;
}

//	FUNCTION TO PUSH RESULTS ONTO VECTOR

void PrintAllPaths::pushResult(std::vector<ObjectId> &path)
{
	m_path.push_back(path);
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
				new_path.push_back(v);
				q.push(new_path);
			}
		}
	}
	return true;
}

IndexType PrintAllPaths::GetResultCount() const
{
	int res = GetResultCountUtility();
	return res;
}

AlgorithmResult PrintAllPaths::GetResult(IndexType index) const
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

IndexType PrintAllPaths::GetHightlightNodesCount() const
{
	return GetResultCountUtility() - 1;
}

ObjectId PrintAllPaths::GetHightlightNode(IndexType index) const
{
	for (auto& subgraph : m_path)
	{
		if (index < subgraph.size())
		{
			return subgraph[index];
		}
		index -= subgraph.size();
	}
}

IndexType PrintAllPaths::GetHightlightEdgesCount() const
{
	return IndexType(GetResultCountUtility() ? GetResultCountUtility() - 2 : 0);
}

NodesEdge PrintAllPaths::GetHightlightEdge(IndexType index) const
{
	NodesEdge edge;
	for (auto& subgraph : m_path)
	{
		if (index < subgraph.size())
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