/**
 *
 *  Interface for algorithm.
 *
 */


#pragma once

#include "IGraph.h"
#include <ostream>

// Edge struct.
struct NodesEdge
{
  ObjectId source;
  ObjectId target;
};

enum AlgorithmParamType {APT_NODE = 0};

struct AlgorithmParam
{
    AlgorithmParamType type;
    char paramName[16];
};

enum AlgorithmResultType {ART_UNKNOWN = 0, ART_INT, ART_FLOAT, ART_STRING};

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
    virtual void SetParameter(const char* name, ObjectId id) = 0;
    // Set graph
    virtual void SetGraph(const IGraph* pGraph) = 0;
    // Calculate algorithm.
    virtual bool Calculate() = 0;
    
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
  // Get result.
  virtual AlgorithmResult GetResult() const = 0;
  // Get propery
  virtual bool GetProperty(ObjectId object, IndexType index, AlgorithmResult* param) const = 0;
  virtual const char* GetPropertyName(IndexType index) const = 0;
    
  virtual ~IAlgorithmResult() {}
};

class IAlgorithm : public IAlgorithmEngine, public IAlgorithmResult 
{
};

