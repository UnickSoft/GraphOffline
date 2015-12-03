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
	// Current graph.
	Graph graph;
	// Report.
	String report;

	// Find shortest path using Dejkstra algorithm.
	DijkstraShortPath* GetShortPath(const String& filename, const String& source, const String& target);

	// Privat structore for params.
	typedef std::map<String, String> ParseCommand;
	
	// Parse command line to private structure.
	void ParseCommandLine (int argc, char *argv[], ParseCommand& commands);

	// Process:  Find short path: -sp graph_filename -start verate_name -finish vertext_name [-report console|xml]
	bool FindShortestPathCommand (int argc, char *argv[]);

	// Create reporter by name.
	IReporter* CreateReporter(const String& reporterName);

public:
	// Main method.
	bool ProcessConsoleParams(int argc, char *argv[]);

	// return last report.
	String GetReport();
};
