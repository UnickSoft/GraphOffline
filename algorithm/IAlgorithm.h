/**
 *
 *  Interface for algorithm.
 *
 */


#pragma once

#include "IGraph.h"
#include <ostream>

class IAlgorithmFactory;

// Edge struct.
struct NodesEdge
{
  ObjectId source;
  ObjectId target;
};

enum AlgorithmParamType {APT_NODE = 0, APT_FLAG};

struct AlgorithmParam
{
    AlgorithmParamType type;
    char paramName[16];
    union
    {
        ObjectId id;
        bool bFlag;
    } data;
};

enum AlgorithmResultType {ART_UNKNOWN = 0, ART_INT, ART_FLOAT, ART_STRING, ART_NODES_PATH};

struct AlgorithmResult
{
    AlgorithmResult () : type(ART_UNKNOWN) {}
    explicit AlgorithmResult(IntWeightType nValue) : type(ART_INT), nValue(nValue) {};
    explicit AlgorithmResult(FloatWeightType fValue) : type(ART_FLOAT), fValue(fValue) {};
    AlgorithmResultType type;
    IntWeightType nValue;
    FloatWeightType fValue;
    char strValue[64];
};

// If path is not avalible, it may have result.
#define INFINITY_PATH_INT INT32_MAX

class IAlgorithmEngine
{
public:
    // Long name of algoright: DijkstraShortPath.
    virtual const char* GetFullName() const = 0;
    // Short name of algorithm: dsp
    virtual const char* GetShortName() const = 0;
    // Enum parameters
    virtual bool EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const = 0;
    // Set parameter to algorithm.
    virtual void SetParameter(const AlgorithmParam* param) = 0;
    // Set graph
    virtual void SetGraph(const IGraph* pGraph) = 0;
    // Calculate algorithm.
    virtual bool Calculate() = 0;
    // Set algorithm factory.
    virtual void SetAlgorithmFactory(const IAlgorithmFactory* pAlgorithmFactory) = 0;
    
    virtual ~IAlgorithmEngine() {}
};

class IAlgorithmResult
{
public:
  // Hightlight nodes count.
  virtual IndexType GetHightlightNodesCount() const = 0;
  // Hightlight node.
  virtual ObjectId GetHightlightNode(IndexType index) const = 0;
  // Hightlight edges count.
  virtual IndexType GetHightlightEdgesCount() const = 0;
  // Hightlight edge.
  virtual NodesEdge GetHightlightEdge(IndexType index) const = 0;
  // Get result count.
  virtual IndexType GetResultCount() const = 0;
  // Get result of index. Algorithms can have complex result.
  virtual AlgorithmResult GetResult(IndexType index) const = 0;
  // Get propery for Node
  virtual bool GetNodeProperty(ObjectId object, IndexType index, AlgorithmResult* param) const = 0;
  virtual const char* GetNodePropertyName(IndexType index) const = 0;

  // Get propery for Edge
  virtual bool GetEdgeProperty(const NodesEdge& object, IndexType index, AlgorithmResult* param) const = 0;
  virtual const char* GetEdgePropertyName(IndexType index) const = 0;
      
  virtual ~IAlgorithmResult() {}
};

class IAlgorithm : public IAlgorithmEngine, public IAlgorithmResult 
{
};


typedef std::shared_ptr<IAlgorithm> AlgorithmPtr;


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

protected:

    const IGraph* m_pGraph;
    const IAlgorithmFactory* m_pAlgorithmFactory;

};
