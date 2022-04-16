/**
 *
 *  Interface for algorithm.
 *
 */


#pragma once

#include "IGraph.h"
#include <cstring>

#include <ostream>
#include <vector>

class IAlgorithmFactory;

// Edge struct.
struct NodesEdge
{
    ObjectId source;
    ObjectId target;
    ObjectId edgeId = 0;
};

inline bool operator < (const NodesEdge& a, const NodesEdge& b)
{
    if (a.source < b.source) return true;
    if (a.source == b.source) return a.target < b.target;
    return false;
}

inline bool operator == (const NodesEdge& a, const NodesEdge& b)
{
    return a.source == b.source && a.target == b.target;
}

enum AlgorithmParamType
{
    APT_NODE = 0,
    APT_NODE_LIST,
    APT_EDGE_LIST,
    APT_FLAG,
    APT_NUMBER,
    APT_STRING
};

#define ALGO_PARAM_STRING_SIZE 16
struct AlgorithmParam
{
    AlgorithmParamType type;
    char paramName[ALGO_PARAM_STRING_SIZE + 1];
    union
    {
        char str[ALGO_PARAM_STRING_SIZE + 1];
        IndexType val;
        ObjectId id;
        bool bFlag;
        ObjectId* ids; // TODO leak
    } data;
};

enum AlgorithmResultType
{
    ART_UNKNOWN = 0,
    ART_INT,
    ART_FLOAT,
    ART_STRING,
    ART_NODES_PATH,
    ART_EDGES_PATH,
    ART_SPLIT_PATHS,
    ART_CLIQUE,
    ART_NODE_ID
};


struct AlgorithmResult
{
#define ALGO_RESULT_STRING_SIZE 64
    AlgorithmResult () : type(ART_UNKNOWN) {}
    explicit AlgorithmResult(IntWeightType nValue) : type(ART_INT), nValue(nValue) {}
    explicit AlgorithmResult(FloatWeightType fValue) : type(ART_FLOAT), fValue(fValue) {}
    explicit AlgorithmResult(const char *str) : type(ART_STRING)
    {
        assert(str != nullptr);
        std::strncpy(strValue, str, ALGO_RESULT_STRING_SIZE);
    }

    AlgorithmResultType type;
    IntWeightType nValue;
    FloatWeightType fValue;
    ObjectId nodeId;
    char strValue[ALGO_RESULT_STRING_SIZE + 1];
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
    // Is support multi graph
    virtual bool IsSupportMultiGraph() const = 0;

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
  virtual bool GetEdgeProperty(const NodesEdge& object, IndexType properyIndex,
    IndexType resultEdgeIndex, AlgorithmResult* param) const = 0;
  virtual const char* GetEdgePropertyName(IndexType index) const = 0;

  virtual ~IAlgorithmResult() {}
};

class IAlgorithm : public IAlgorithmEngine, public IAlgorithmResult
{
public:
    virtual void UnitTest() const = 0;
};


typedef std::shared_ptr<IAlgorithm> AlgorithmPtr;
