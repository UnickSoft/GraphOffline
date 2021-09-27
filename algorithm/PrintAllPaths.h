#ifndef PRIMS_ALGORITHM_CPP
#define PRIMS_ALGORITHM_CPP


#include "BaseAlgorithm.h"
#include "IGraph.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <stdio.h>

class PrintAllPaths : public BaseAlgorithm
{
protected:
	ObjectId m_source;		// OBJECT ID STORING SOURCE
	ObjectId m_target;		// OBJECT ID STORING DESTINATION
	std::vector<std::vector<ObjectId>> m_path;	//	VECTOR THAT STORES FINAL RESULT

	void pushResult(std::vector<ObjectId> &path);	// UTITLITY FUNCTION TO PUSH RESULTS ON m_path
	IndexType GetResultCountUtility() const;		// UTITLITY FUNCTION THAT COUNTS NUMBER OF ELEMENTS IN RESULT

public:
	PrintAllPaths();	

	virtual ~PrintAllPaths() = default;

	// OVERRIDEN FUNCTION FROM BASE ALGORITHM

	//	RETURNS FULL NAME OF THE ALGORITHM
	virtual const char* GetFullName() const override { return "PrintAllPaths"; }

	//	RETURNS SHORT NAME OF THE ALGORITHM
	virtual const char* GetShortName() const override { return "prnpaths"; }

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
};

#endif