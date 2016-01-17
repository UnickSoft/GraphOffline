/**
 *  Interface for algorithm.
 */


//#include "DijkstraShortPath.h"
#include <map>
#include <cstddef>
#include <algorithm>    // std::reverse
#include <string.h>
#include <list>

static const char* g_lowestDistanceStr = "lowestDistance";
static const char* g_indexStr = "index";

template<class WeightTypeInterface, typename WeightType> DijkstraShortPath<WeightTypeInterface, WeightType>::DijkstraShortPath ()
{
    m_source = 0;
    m_target = 0;
    m_pGraph = NULL;
    m_result = INFINITY_PATH_INT;
    
#if __GNUC__ > 4
    // We support only 2 types;
    static_assert((sizeof(WeightType) == sizeof(FloatWeightType)) || (sizeof(WeightType) == sizeof(IntWeightType)));
#endif
}

template<class WeightTypeInterface, typename WeightType> DijkstraShortPath<WeightTypeInterface, WeightType>::~DijkstraShortPath ()
{

}

// Enum parameters
template<class WeightTypeInterface, typename WeightType> bool DijkstraShortPath<WeightTypeInterface, WeightType>::EnumParameter(uint32_t index, AlgorithmParam* outParamInfo) const
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

// Set parameter to algorithm.
template<class WeightTypeInterface, typename WeightType> void DijkstraShortPath<WeightTypeInterface, WeightType>::SetParameter(const AlgorithmParam* outParamInfo)
{
    if (outParamInfo)
    {
        if (strncmp(outParamInfo->paramName, "start", sizeof(AlgorithmParam().paramName)) == 0)
        {
            m_source = outParamInfo->data.id;
        }
        else if (strncmp(outParamInfo->paramName, "finish", sizeof(AlgorithmParam().paramName)) == 0)
        {
            m_target = outParamInfo->data.id;
        }
    }
}

// Calculate algorithm.
template<class WeightTypeInterface, typename WeightType> bool DijkstraShortPath<WeightTypeInterface, WeightType>::Calculate()
{
    bool res = false;
    // Look algorithm: http://en.wikipedia.org/wiki/Dijkstra's_algorithm.
    if (m_pGraph && m_source && m_target)
    {
        std::unordered_map<ObjectId, WeightType> dist;
        std::unordered_map<ObjectId, ObjectId> previous;
        std::list<ObjectId> q;
        
        for (size_t i = 0; i < m_pGraph->GetNodesCount(); i ++)
        {
            ObjectId object  = m_pGraph->GetNode(i);
            dist[object]     = 1E9;
            previous[object] = 0;
            q.push_back(object);
        }
        
        dist[m_source] = 0;
        
        while (q.size() > 0)
        {
            int index = -1;
            WeightType nMinFromQ  = 1E9;
            
            auto uIterator = q.begin();
            for (auto element = q.begin(); element != q.end(); element++)
            {
                if (nMinFromQ > dist[*element])
                {
                    nMinFromQ = dist[*element];
                    uIterator = element;
                    //break;
                }
            }
            ObjectId u = *uIterator;
            q.erase(uIterator);
            
            for (IndexType i = 0; i < m_pGraph->GetConnectedNodes(u); i++)
            {
                ObjectId v = m_pGraph->GetConnectedNode(u, i);
                WeightType alt = dist[u] + m_pGraph->GetEdgeWeight(u, v);
                if (alt < dist[v])
                {
                    dist[v]     = alt;
                    previous[v] = u;
                }
            }
        }
        
        ObjectId u = m_target;
        m_path.clear();
        
        m_lowestDistance = dist;
        
        if (previous[u] != NULL)
        {
            m_result = 0;
        }
        while (previous[u] != NULL)
        {
            m_path.push_back(u);
            m_result += m_pGraph->GetEdgeWeight(previous[u], u);
            u = previous[u];
        }
        if (m_path.size() > 0)
        {
            m_path.push_back(m_source);
        }
        
        std::reverse(m_path.begin(),m_path.end());
        
        res = true;
    }
    
    return res;
}

// Hightlight nodes count.
template<class WeightTypeInterface, typename WeightType> IndexType DijkstraShortPath<WeightTypeInterface, WeightType>::GetHightlightNodesCount() const
{
  return m_path.size();
}

// Hightlight node.
template<class WeightTypeInterface, typename WeightType> ObjectId DijkstraShortPath<WeightTypeInterface, WeightType>::GetHightlightNode(IndexType index) const
{
  return m_path[index];
}

// Hightlight edges count.
template<class WeightTypeInterface, typename WeightType> IndexType DijkstraShortPath<WeightTypeInterface, WeightType>::GetHightlightEdgesCount() const
{
  return m_path.size() ? m_path.size() - 1 : 0;
}

// Hightlight edge.
template<class WeightTypeInterface, typename WeightType> NodesEdge DijkstraShortPath<WeightTypeInterface, WeightType>::GetHightlightEdge(IndexType index) const
{
  NodesEdge edge;
  edge.source = m_path[index];
  edge.target = m_path[index + 1];
  return edge;
}

// Get result count.
template<class WeightTypeInterface, typename WeightType>  IndexType DijkstraShortPath<WeightTypeInterface, WeightType>::GetResultCount() const
{
    return 1 + m_path.size();
}

template<class WeightTypeInterface, typename WeightType> AlgorithmResult DijkstraShortPath<WeightTypeInterface, WeightType>::GetResult(IndexType index) const
{
    AlgorithmResult result;

    if (index == 0)
    {
        if (typeid(WeightType) == typeid(FloatWeightType))
        {
            result.type = ART_FLOAT;
            result.fValue = (FloatWeightType) m_result;
        }
        else
        {
            result.type = ART_INT;
            result.nValue = (IntWeightType) m_result;
        }
    }
    else if (index < m_path.size() + 1)
    {
        result.type = ART_NODES_PATH;
        m_pGraph->GetNodeStrId(m_path[index - 1], result.strValue,
                               sizeof(result.strValue));
    }
    
    return result;
}

template<class WeightTypeInterface, typename WeightType> bool DijkstraShortPath<WeightTypeInterface, WeightType>::GetProperty(ObjectId object, IndexType index, AlgorithmResult* param) const
{
    bool result = false;
    
    if (m_lowestDistance.count(object) > 0 && index == 0 && param)
    {
        if (typeid(WeightType) == typeid(FloatWeightType))
        {
            param->type = ART_FLOAT;
            param->fValue = (FloatWeightType) m_lowestDistance.at(object);
        }
        else
        {
            param->type = ART_INT;
            param->nValue = (IntWeightType) m_lowestDistance.at(object);
        }
        result = true;
    }
    if (index == 1 && param)
    {
        auto position = std::find(m_path.begin(), m_path.end(), object);
        
        if (position != m_path.end())
        {
            param->type = ART_INT;
            param->nValue = (IntWeightType)(position - m_path.begin());
            result = true;
        }
    }

    return result;
}

// Set graph
template<class WeightTypeInterface, typename WeightType> void DijkstraShortPath<WeightTypeInterface, WeightType>::SetGraph(const IGraph* pGraph)
{
    //(TODO)
    m_pGraph = dynamic_cast<const WeightTypeInterface*>(pGraph);
}

template<class WeightTypeInterface, typename WeightType> const char* DijkstraShortPath<WeightTypeInterface, WeightType>::GetPropertyName(IndexType index) const
{
    if (index == 0)
    {
        return g_lowestDistanceStr;
    }
    else if (index == 1)
    {
        return g_indexStr;
    }
    else
    {
        return nullptr;
    }
}

template<class WeightTypeInterface, typename WeightType> void DijkstraShortPath<WeightTypeInterface, WeightType>::SetAlgorithmFactory(const IAlgorithmFactory* pAlgorithmFactory)
{
    
}
