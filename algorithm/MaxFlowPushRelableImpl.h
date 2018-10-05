/**
 *  Interface for algorithm.
 */


//#include "DijkstraShortPath.h"
#include <map>
#include <cstddef>
#include <algorithm>    // std::reverse
#include <string.h>
#include <list>

static const char* g_flowValueStr = "flowValue";
static const char* g_backTpFrontValueStr = "backToFront";


template<class WeightTypeInterface, typename WeightType> MaxFlowPushRelabel<WeightTypeInterface, WeightType>::MaxFlowPushRelabel ()
{
    _source = 0;
    _drain = 0;
    _pGraph = NULL;
    m_result = 0;
    
#if __GNUC__ > 4
    // We support only 2 types;
    static_assert((sizeof(WeightType) == sizeof(FloatWeightType)) || (sizeof(WeightType) == sizeof(IntWeightType)));
#endif
}


// Enum parameters
template<class WeightTypeInterface, typename WeightType> bool MaxFlowPushRelabel<WeightTypeInterface, WeightType>::EnumParameter(uint32_t index, AlgorithmParam* outParamInfo) const
{
    bool res = false;
    
    switch (index)
    {
        case 0:
        {
            strncpy(outParamInfo->paramName, "source", sizeof(outParamInfo->paramName));
            outParamInfo->type = APT_NODE;
            res = true;
            break;
        }
        case 1:
        {
            strncpy(outParamInfo->paramName, "drain", sizeof(outParamInfo->paramName));
            outParamInfo->type = APT_NODE;
            res = true;
            break;
        }
    }
    
    return res;
}

// Set parameter to algorithm.
template<class WeightTypeInterface, typename WeightType> void MaxFlowPushRelabel<WeightTypeInterface, WeightType>::SetParameter(const AlgorithmParam* outParamInfo)
{
    if (outParamInfo)
    {
        if (strncmp(outParamInfo->paramName, "source", sizeof(AlgorithmParam().paramName)) == 0)
        {
            _source = outParamInfo->data.id;
        }
        else if (strncmp(outParamInfo->paramName, "drain", sizeof(AlgorithmParam().paramName)) == 0)
        {
            _drain = outParamInfo->data.id;
        }
    }
}

// Calculate algorithm.
template<class WeightTypeInterface, typename WeightType> bool MaxFlowPushRelabel<WeightTypeInterface, WeightType>::Calculate()
{
    bool res = false;
    _flowValue.clear();
    m_result = 0;
    
    // Look algorithm: https://en.wikipedia.org/wiki/Push–relabel_maximum_flow_algorithm
    // http://acm.mipt.ru/twiki/bin/view/Algorithms/MaxFlowPushRelabelCPP
    if (_pGraph && _source && _drain && _pGraph->GetNodesCount() > 0)
    {
        auto n = _pGraph->GetNodesCount();
        std::vector<WeightType> excessFlow;
        std::vector<std::vector<WeightType>> adjacencyMatrix = this->GetAdjacencyMatrix<WeightType, WeightTypeInterface>(*_pGraph);
        std::vector<std::vector<WeightType>> origin_adjacencyMatrix;
        std::vector<WeightType> height;
        
        
        auto push_func = [&excessFlow, &adjacencyMatrix](int u,int v)
        {
            WeightType f = std::min(excessFlow[u], adjacencyMatrix[u][v]);
            excessFlow[u] -= f;
            excessFlow[v] += f;
            adjacencyMatrix[u][v] -= f;
            adjacencyMatrix[v][u] += f;
        };

        auto lift_func = [&height, n, &adjacencyMatrix](int u)
        {
            int min = 3 * n + 1;

            for (int i = 0; i < n; i++)
            {
                if (adjacencyMatrix[u][i] && (height[i] < min))
                {
                    min = height[i];
                }
            }
            height[u] = min + 1;
        };

        auto discharge_func = [&excessFlow, &height, &push_func, &lift_func, &adjacencyMatrix, n](int u)
        {
            int v = 0;
            while (excessFlow[u] > 0)
            {
                if (adjacencyMatrix[u][v] && height[u] == height[v] + 1)
                {
                    push_func(u, v);
                    v = 0;
                    continue;
                }
                v++;
                if (v == n)
                {
                    lift_func(u);
                    v = 0;
                }
            }
        };
        
        height.resize(n);
        
        int sourceIndex = 0;
        int drainIndex  = 0;
        
        for (IndexType i = 0; i < n; i++)
        {
            if (_pGraph->GetNode(i) == _source)
            {
                sourceIndex = i;
            }
            
            if (_pGraph->GetNode(i) == _drain)
            {
                drainIndex = i;
            }
        }
        
        origin_adjacencyMatrix = adjacencyMatrix;
        
        
        // Init
        excessFlow.resize(n);
        for (IndexType i = 0; i < _pGraph->GetNodesCount(); i++)
        {
            if (_pGraph->GetNode(i) == sourceIndex)
              continue;

            excessFlow[i] = adjacencyMatrix[sourceIndex][i];
            adjacencyMatrix[i][sourceIndex] += adjacencyMatrix[sourceIndex][i];
        }
        
        height[sourceIndex] = n;
        
        
        // Start
        std::list<int> l;
        int old = 0;
        
        for (int i = 0; i < n; i++)
        {
            if (i != sourceIndex && i != drainIndex)
            {
                l.push_front(i);
            }
        }
        
        
        auto cur = l.begin();
        
        // Calculate
        while (cur != l.end())
        {
            old = height[*cur];
            
            discharge_func(*cur);
            
            if (height[*cur] != old)
            {
                l.push_front(*cur);
                l.erase(cur);
                cur = l.begin();
            }
            cur++;
        }
        
        m_result = excessFlow[drainIndex];
        
        findFlowValues(drainIndex, sourceIndex, origin_adjacencyMatrix, adjacencyMatrix);
        
        // debugPring();
        res = true;
    }
    
    return res;
}

// Hightlight edges count.
template<class WeightTypeInterface, typename WeightType> IndexType MaxFlowPushRelabel<WeightTypeInterface, WeightType>::GetHightlightEdgesCount() const
{
  return _flowValue.size() ? (IndexType)_flowValue.size() : 0;
}

// Hightlight edge.
template<class WeightTypeInterface, typename WeightType> NodesEdge MaxFlowPushRelabel<WeightTypeInterface, WeightType>::GetHightlightEdge(IndexType index) const
{
  return _flowValue[index].edge;
}

// Get result count.
template<class WeightTypeInterface, typename WeightType>  IndexType MaxFlowPushRelabel<WeightTypeInterface, WeightType>::GetResultCount() const
{
    return 1;
}

template<class WeightTypeInterface, typename WeightType> AlgorithmResult MaxFlowPushRelabel<WeightTypeInterface, WeightType>::GetResult(IndexType index) const
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
    
    return result;
}

template<class WeightTypeInterface, typename WeightType> bool MaxFlowPushRelabel<WeightTypeInterface, WeightType>::GetEdgeProperty(const NodesEdge& object, IndexType index, AlgorithmResult* param) const
{
    bool result = false;
    
    if ((index == 0 || index == 1) && param)
    {
        auto findRes = std::find_if(_flowValue.begin(), _flowValue.end(), [&object](const EdgeFlowValue& edge)
            {
                return edge.edge.source == object.source && edge.edge.target == object.target;
            });
        
        bool found   = findRes != _flowValue.end();
        
        if (index == 0)
        {
            if (typeid(WeightType) == typeid(FloatWeightType))
            {
                param->type   = ART_FLOAT;
                param->fValue = found ? (FloatWeightType) findRes->value : 0.0;
            }
            else
            {
                param->type   = ART_INT;
                param->nValue = found ? (IntWeightType) findRes->value : 0;
            }
        }
        else if (index == 1)
        {
                param->type   = ART_INT;
                param->nValue = found ? (IntWeightType) findRes->backToFront : 0;
        }
        
        result = true;
    }

    
    return result;
}

// Set graph
template<class WeightTypeInterface, typename WeightType> void MaxFlowPushRelabel<WeightTypeInterface, WeightType>::SetGraph(const IGraph* pGraph)
{
    //(TODO)
    _pGraph = dynamic_cast<const WeightTypeInterface*>(pGraph);
}

template<class WeightTypeInterface, typename WeightType> const char* MaxFlowPushRelabel<WeightTypeInterface, WeightType>::GetEdgePropertyName(IndexType index) const
{
    if (index == 0)
    {
        return g_flowValueStr;
    }
    else if (index == 1)
    {
        return g_backTpFrontValueStr;
    }
    else
    {
        return nullptr;
    }
}


template<class WeightTypeInterface, typename WeightType> void MaxFlowPushRelabel<WeightTypeInterface, WeightType>::findFlowValues(IndexType drainIndex, IndexType sourceIndex, std::vector<std::vector<WeightType>>& origin_adjacencyMatrix, std::vector<std::vector<WeightType>>& adjacencyMatrix)
{
    auto n = _pGraph->GetNodesCount();
    // Find flow for edges
    std::list<int> backQ;
    backQ.push_back(drainIndex);
    
    std::vector<WeightType> realFlow;
    realFlow.resize(n);
    std::fill(realFlow.begin(), realFlow.end(), 0);
    realFlow[drainIndex]  = m_result;
    realFlow[sourceIndex] = m_result;
    
    while (!backQ.empty())
    {
        auto currentNode = backQ.front();
        backQ.pop_front();
        auto& currentFlow = realFlow[currentNode];
        for (IndexType i = 0; i < n; i++)
        {
            if (i != currentNode && origin_adjacencyMatrix[i][currentNode] > 0)
            {
                auto deltaFlow = (i != sourceIndex) ? - adjacencyMatrix[i][currentNode] + origin_adjacencyMatrix[i][currentNode] : origin_adjacencyMatrix[i][currentNode];
                
                if (deltaFlow > 0)
                {
                    EdgeFlowValue edge;
                    auto s = _pGraph->GetNode(i);
                    auto d = _pGraph->GetNode(currentNode);
                    
                    edge.edge.source = s;
                    edge.edge.target = d;
                    edge.value       = std::min(currentFlow, deltaFlow);
                    
                    if (i != sourceIndex)
                    {
                        adjacencyMatrix[i][currentNode] += edge.value;
                    }
                    else
                    {
                        origin_adjacencyMatrix[i][currentNode] -= edge.value;
                    }
                    //printf ("%d -> %d = %f\n", i, currentNode, edge.value);
                    
                    currentFlow -= edge.value;
                    
                    _flowValue.push_back(edge);
                    
                    if (i != sourceIndex)
                    {
                        realFlow[i] += edge.value;
                        backQ.push_back(i);
                    }
                }
            }
            
            if (currentFlow <= 0)
            {
                break;
            }
        }
    }
    
    // Remove dublications.
    std::sort(_flowValue.begin(), _flowValue.end(), [](const EdgeFlowValue& a, const EdgeFlowValue& b){
            return a.edge < b.edge;
    });
    
    auto flowValueCopy = _flowValue;
    
    auto last = std::unique(_flowValue.begin(), _flowValue.end(), [](const EdgeFlowValue& a, const EdgeFlowValue& b){
            return a.edge == b.edge;
    });
    
    _flowValue.erase(last, _flowValue.end());
    
    for(auto element = _flowValue.begin(); element != last; element++)
    {
        element->value = 0;
        for (auto comapare = flowValueCopy.begin(); comapare != flowValueCopy.end(); comapare++)
        {
            if (element->edge.source == comapare->edge.source && element->edge.target == comapare->edge.target)
            {
                element->value += comapare->value;
            }
        }
    }
    
    std::for_each(_flowValue.begin(), _flowValue.end(), [this](EdgeFlowValue& edge){
        if (!_pGraph->IsEgdeExists(edge.edge.source, edge.edge.target))
        {
            std::swap(edge.edge.source, edge.edge.target);
            edge.backToFront = true;
        }
    });
}


template<class WeightTypeInterface, typename WeightType> void MaxFlowPushRelabel<WeightTypeInterface, WeightType>::debugPring()
{
    for (auto& edge : _flowValue)
    {
        char s[255] = {};
        char d[255] = {};
        
        _pGraph->GetNodeStrId(edge.edge.source, s, 255);
        _pGraph->GetNodeStrId(edge.edge.target, d, 255);
        
        printf ("%s -> %s = %f\n", s, d, edge.value);
    }
}
