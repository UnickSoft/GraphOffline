/**
 *
 *  Interface for algorithm.
 *
 */


#pragma once

#include "IAlgorithm.h"
#include <ostream>

class BaseAlgorithm : public IAlgorithm
{
public:
  BaseAlgorithm() : m_pGraph(NULL), m_pAlgorithmFactory(NULL) {}

  IndexType GetHightlightNodesCount() const override {return 0;}
  ObjectId GetHightlightNode(IndexType index) const override {return 0;}
  IndexType GetHightlightEdgesCount() const override {return 0;}
  NodesEdge GetHightlightEdge(IndexType index) const override {return NodesEdge();}
  IndexType GetResultCount() const override {return 0;}
  AlgorithmResult GetResult(IndexType index) const override {return AlgorithmResult();}
  bool GetNodeProperty(ObjectId object, IndexType index, AlgorithmResult* param) const override {return false;}
  const char* GetNodePropertyName(IndexType index) const override {return 0;}
  bool GetEdgeProperty(const NodesEdge& object, IndexType index, AlgorithmResult* param) const override {return false;}
  const char* GetEdgePropertyName(IndexType index) const override {return 0;}
  
  bool EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const override {return false;};
  void SetParameter(const AlgorithmParam* param) override {};
  void SetGraph(const IGraph* pGraph) override {m_pGraph = pGraph;}
  void SetAlgorithmFactory(const IAlgorithmFactory* pAlgorithmFactory) override {m_pAlgorithmFactory = pAlgorithmFactory;}
  
  template<class WeightType, class GraphType> static std::vector<std::vector<WeightType>> GetAdjacencyMatrix(const GraphType& graph);

  static std::vector<std::vector<bool>> GetAdjacencyMatrixBool(const IGraph& graph);

protected:

    const IGraph* m_pGraph;
    const IAlgorithmFactory* m_pAlgorithmFactory;

};

template<class WeightType, class GraphType> std::vector<std::vector<WeightType>> BaseAlgorithm::GetAdjacencyMatrix(const GraphType& graph)
{
    std::vector<std::vector<WeightType>> res;
    
    res.resize(graph.GetNodesCount());
    
    auto nodesCount = graph.GetNodesCount();
    
    for (IndexType i = 0; i < nodesCount; i++)
    {
        res[i].resize(nodesCount);
        
        for (IndexType j = 0; j < nodesCount; j++)
        {
            auto s = graph.GetNode(i);
            auto f = graph.GetNode(j);
            if (graph.IsEgdeExists(s, f, false))
            {
                res[i][j] = graph.GetEdgeWeight(s, f);
            }
            else
            {
                res[i][j] = 0;
            }
        }
    }
    
    return res;
}
