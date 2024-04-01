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
	for (auto order_vertex : visited)
	{
        if (m_source == order_vertex) {
            continue;
        }
		m_path.push_back(paths[order_vertex]);
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
    using ObjectsSet = std::unordered_set<ObjectId>;

    // Use this for algorithm steps.
    // Swap steps.
    struct StepData 
    {
        // Current distance to vertex
        std::unordered_map<ObjectId, WeightType> dist;
        // Fix negative loops, save vertex in current distance.
        std::unordered_map<ObjectId, ObjectsSet> inDist;
        
        // Save all path to vertex
	    std::unordered_map<ObjectId, std::vector<ObjectId>> path;	
	    std::set<ObjectId> visited;
    };
    
    std::vector<StepData> steps;
    steps.resize(2);
    
	bool res    = false;	//	VARIABLE CHECKING IF CALCULATION IS POSSIBLE, RETURN VALUE
	bool change = true;	//	VARIABLE USED FOR CHECKING ANY CHANGE IN ITERATION

    uint32_t currentStep = 0;
    
    {
        auto& step = steps[currentStep];
        for(int i = 0; i < m_pGraph->GetNodesCount(); i++)
        {
            ObjectId u = m_pGraph->GetNode((IndexType)i);
            step.dist[u] = (WeightType)std::numeric_limits<WeightType>::max();
        }
        ObjectId u = m_pGraph->GetNode((IndexType)0);
        step.dist[m_source] = 0;
        step.inDist[m_source].insert(m_source);
        step.visited.insert(m_source);
        step.path[m_source].push_back(m_source);
    }
    
    while (change)
    {
        change = false;
        auto& prevStep = steps[currentStep];
        currentStep    = 1 - currentStep;
        auto& step     = steps[currentStep];
        step = prevStep;
        
        for (auto u : prevStep.visited)
        {
            for (IndexType k = 0; k < m_pGraph->GetConnectedNodes(u); k++)
            {
                ObjectId v = m_pGraph->GetConnectedNode(u, k);
                if (m_source == v)
                {
                    continue;
                }

                WeightType alt = m_pGraph->GetEdgeWeight(u, v);
                if (alt + prevStep.dist[u] < prevStep.dist[v] && prevStep.inDist[u].find(v) == prevStep.inDist[u].end())
                {
                    step.path[v] = prevStep.path[u];
                    step.path[v].push_back(v);
                    step.dist[v] = alt + prevStep.dist[u];
                    auto inPrevDistance = prevStep.inDist[u];
                    inPrevDistance.insert(v);
                    step.inDist[v] = inPrevDistance;
                    change = true;
                    res = true;
                    if (step.visited.size() < m_pGraph->GetNodesCount())
                    {
                        step.visited.insert(v);
                    }
                }
            }
        }
    }

    // Take result from the last step
    {
        auto& step     = steps[currentStep];
        paths   = step.path;
	    visited = step.visited;
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
