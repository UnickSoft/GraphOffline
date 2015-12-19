/**
 * Process console params.
 *
 */

#pragma once

#include "GraphMLReporter.h"
#include "ConsoleReporter.h"
#include "YString.h"
#include "Graph.h"
#include <map>
#include "DijkstraShortPath.h"
#include "AlgorithmFactory.h"

/**
 *  Console parameters of GraphUtility:
 *  Find short path: -sp graph_filename -start verate_name -finish vertext_name [-report console|xml]
 *
 *
 *
 *
 */

class ConsoleParams
{
private:
	// Report.
	String report;

	// Find shortest path using Dejkstra algorithm.
	DijkstraShortPath* GetShortPath(const String& filename, const String& source, const String& target);

	
	// Parse command line to private structure.
	void ParseCommandLine (const std::vector<String>& params, ParametersMap& commands);

	// Process:  Find short path: -sp graph_filename -start verate_name -finish vertext_name [-report console|xml]
	bool FindShortestPathCommand (int argc, char *argv[]);
    
    // Create reporter by name.
    std::shared_ptr<IReporter> CreateReporter(const String& reporterName);
    
    // Load graph from file or from buffer.
    bool LoadGraph(const String& soirceName, Graph& graph);
    
    // Get real param value.
    String GetRealParamName(const String& paramName);
    
    // @return utility help.
    String GetHelp();
    
public:
    
	// Main method.
    bool ProcessConsoleParams(const std::vector<String>& params);

	// return last report.
	String GetReport();
};
