#pragma once

#include "Graph.h"
#include "YString.h"

class CGIProcessor
{
private:
	// Current graph.
	Graph graph;
	// Report.
	String report;

public:
	CGIProcessor(void);
	~CGIProcessor(void);

	// Process CGI request.
	bool Process();

	// Return report after process.
	String GetReport();
};
