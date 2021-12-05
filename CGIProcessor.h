#pragma once

#include "Graph.h"
#include "YString.h"

class CGIProcessor
{
public:
	// Return params of this call.
    std::vector<String> GetRequestParams();
    // Return buffer of graph.
    String GetGraphBuffer();

    // Split string using delemiters.
    static std::vector<String> SplitString(const String& inputString, const std::vector<String>& delemiters);

private:
    
    String m_graphBuffer;
    std::vector<String> m_parameters;
};
