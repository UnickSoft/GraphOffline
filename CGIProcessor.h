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

    
private:
    
    String m_graphBuffer;
    std::vector<String> m_parameters;
    
    static std::vector<String> SplitString(const String& inputString, const std::vector<String>& delemiters);
};
