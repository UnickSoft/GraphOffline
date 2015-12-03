/**
 *  Interface for algorithm.
 */


#pragma once

#include "DijkstraShortPath.h"
#include <map>
#include <cstddef>
#include <algorithm>    // std::reverse
#include <string.h>
#include <list>

DijkstraShortPath::DijkstraShortPath (ObjectId source, ObjectId target, IGraph* pGraph)
{
  m_source = source;
  m_target = target;
  m_pGraph = pGraph;
  m_result = INFINITY_PATH_INT;
}

DijkstraShortPath::~DijkstraShortPath ()
{

}

// Calculate algorithm.
void DijkstraShortPath::Calculate()
{
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

      for (int i = 0; i < m_pGraph->GetConnectedGraphs(u); i++)
      {
        ObjectId v = m_pGraph->GetConnectedGraph(u, i);
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
  }
}

// Hightlight nodes count.
unsigned int DijkstraShortPath::GetHightlightNodesCount()
{
  return m_path.size();
}

// Hightlight node.
ObjectId DijkstraShortPath::GetHightlightNode(int index)
{
  return m_path[index];
}

// Hightlight edges count.
unsigned int DijkstraShortPath::GetHightlightEdgesCount()
{
  return m_path.size() ? m_path.size() - 1 : 0;
}

// Hightlight edge.
NodesEdge DijkstraShortPath::GetHightlightEdge(int index)
{
  NodesEdge edge;
  edge.source = m_path[index];
  edge.target = m_path[index + 1];
  return edge;
}

int DijkstraShortPath::GetResult()
{
  return m_result;
}

int DijkstraShortPath::GetProperty(ObjectId object, const char* name)
{
	return (strcmp(name, "lowestDistance") == 0 ? m_lowestDistance[object] : 0);
}