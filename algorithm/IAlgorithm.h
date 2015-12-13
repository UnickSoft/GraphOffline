/**
 *
 *  Interface for algorithm.
 *
 */


#pragma once

#include "IGraph.h"

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
  virtual IntWeightType GetResult() const = 0;
  // Get propery
  virtual IntWeightType GetProperty(ObjectId object, const char* name) const = 0;
    
  virtual ~IAlgorithmResult() {}
};

class IAlgorithm : public IAlgorithmEngine, public IAlgorithmResult 
{
};
