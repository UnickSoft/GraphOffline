/**
 *  Interface for algorithm.
 */


#include "DijkstraShortPath.h"
#include <map>
#include <cstddef>
#include <algorithm>    // std::reverse
#include <string.h>
#include <list>

DijkstraShortPath::DijkstraShortPath (IGraph* pGraph)
{
  m_source = 0;
  m_target = 0;
  m_pGraph = pGraph;
  m_result = INFINITY_PATH_INT;
}

DijkstraShortPath::~DijkstraShortPath ()
{

}

// Enum parameters
bool DijkstraShortPath::EnumParameter(uint32_t index, AlgorithmParam* outParamInfo) const
{
    bool res = false;
    
    switch (index)
    {
        case 0:
        {
            strncpy(outParamInfo->paramName, "start", sizeof(outParamInfo->paramName));
            res = true;
        }
        case 1:
        {
            strncpy(outParamInfo->paramName, "finish", sizeof(outParamInfo->paramName));
            res = true;
        }
    }
    
    return res;
}

// Set parameter to algorithm.
void DijkstraShortPath::SetParameter(const char* name, ObjectId id)
{
    if (strncmp(name, "start", sizeof(AlgorithmParam::paramName)) == 0)
    {
        m_source = id;
    }
    else if (strncmp(name, "finish", sizeof(AlgorithmParam::paramName)) == 0)
    {
        m_target = id;
    }
}

// Calculate algorithm.
bool DijkstraShortPath::Calculate()
{
    bool res = false;
    // Look algorithm: http://en.wikipedia.org/wiki/Dijkstra's_algorithm.
    if (m_pGraph && m_source && m_target)
    {
        std::unordered_map<ObjectId, int> dist;
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
            int nMinFromQ  = 1E9;
            
            auto uIterator = q.begin();
            for (auto element = q.begin(); element != q.end(); element++)
            {
                if (nMinFromQ > dist[*element])
                {
                    nMinFromQ = dist[*element];
                    uIterator = element;
                    break;
                }
            }
            ObjectId u = *uIterator;
            q.erase(uIterator);
            
            for (IndexType i = 0; i < m_pGraph->GetConnectedNodes(u); i++)
            {
                ObjectId v = m_pGraph->GetConnectedNode(u, i);
                int alt = dist[u] + m_pGraph->GetEdgeWeight(u, v);
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
IndexType DijkstraShortPath::GetHightlightNodesCount() const
{
  return m_path.size();
}

// Hightlight node.
ObjectId DijkstraShortPath::GetHightlightNode(IndexType index) const
{
  return m_path[index];
}

// Hightlight edges count.
IndexType DijkstraShortPath::GetHightlightEdgesCount() const
{
  return m_path.size() ? m_path.size() - 1 : 0;
}

// Hightlight edge.
NodesEdge DijkstraShortPath::GetHightlightEdge(IndexType index) const
{
  NodesEdge edge;
  edge.source = m_path[index];
  edge.target = m_path[index + 1];
  return edge;
}

int DijkstraShortPath::GetResult() const
{
  return m_result;
}

int DijkstraShortPath::GetProperty(ObjectId object, const char* name) const
{
	return (strcmp(name, "lowestDistance") == 0 ? m_lowestDistance.at(object) : 0);
}

