#pragma once

#include "Graph.h"
#include "YString.h"

class CGIProcessor
{
public:
	// Return params of this call.
	static std::vector<String> GetRequestParams();
    // Return buffer of graph.
    static String GetGraphBuffer();

    
private:
    
    static std::vector<String> SplitString(const String& inputString, const std::vector<String>& delemiters);
};
