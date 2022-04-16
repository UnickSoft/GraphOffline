#pragma once

#include "BaseAlgorithm.h"
#include "IGraph.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <stdio.h>
#include <set>

template<class WeightTypeInterface, class WeightType> class BellmanFord : public BaseAlgorithm
{
protected:
	ObjectId m_source;		// OBJECT ID STORING SOURCE
	std::vector<std::vector<ObjectId>> m_path;	//	VECTOR THAT STORES FINAL RESULT
	std::unordered_map<ObjectId, ObjectId> previous_vertex;	//	VARIABLE USED TO STORE THE PREVIOUS ELEMENT OF SHORTEST PATH
	std::set<ObjectId> previous_vertex_order;
	const WeightTypeInterface*  m_pGraph;

	void pushResult();	// UTITLITY FUNCTION TO PUSH RESULTS ON m_path
	IndexType GetResultCountUtility() const;		// UTITLITY FUNCTION THAT COUNTS NUMBER OF ELEMENTS IN RESULT

public:
	BellmanFord();

	virtual ~BellmanFord() = default;

	// OVERRIDEN FUNCTION FROM BASE ALGORITHM

	//	RETURNS FULL NAME OF THE ALGORITHM
	virtual const char* GetFullName() const override { return "BellmanFord"; }

	//	RETURNS SHORT NAME OF THE ALGORITHM
	virtual const char* GetShortName() const override { return "blf"; }

	//	SUPPORTS MULTI GRAPH
	virtual bool IsSupportMultiGraph() const override { return true; }

	//	GET ALL THE PARAMETERS
	virtual bool EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const override;

	virtual void SetParameter(const AlgorithmParam* param) override;

	//	FUNCTION RESPONSIBLE FOR FINDING SHORTEST PATH, RETURNS TRUE IF SUCCESSFUL
	virtual bool Calculate() override;

	//	RETURN THE TOTAL NUMBER OF ELEMENTS IN RESULT
	virtual IndexType GetResultCount() const override;

	//	FUNCTION TO WRITE RESULT THROUGH ALGORITHM RESULT
	virtual AlgorithmResult GetResult(IndexType index) const override;

	//	GETS THE NUMBER OF NODES TO BE HIGHLIGHTED
	virtual IndexType GetHightlightNodesCount() const override;

	//	RETURNS THE OBJECTID OF NODE TO BE HIGHLIGHTED
	virtual ObjectId GetHightlightNode(IndexType index) const override;

	//	GETS THE NUMBER OF EDGES TO BE HIGHLIGHTED
	virtual IndexType GetHightlightEdgesCount() const override;

	//	RETURNS THE EDGE TO BE HIGHLIGHTED
	virtual NodesEdge GetHightlightEdge(IndexType index) const override;

	//	SET GRAPH
	virtual void SetGraph(const IGraph* pGraph) override;

    virtual void UnitTest() const override {}
};

#include "BellmanFordImpl.h"
