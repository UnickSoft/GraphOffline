#include "CGIProcessor.h"
#include "DijkstraShortPath.h"
#include "GraphMLReporter.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <memory.h> 


std::vector<String> CGIProcessor::GetRequestParams()
{
    std::vector<String> res = m_parameters;

    if (res.empty())
    {
        char * requestMethod = getenv("REQUEST_METHOD");
        if (strstr(requestMethod, "POST"))
        {
            // "REQUEST_STRING" should have start=n0&finish=n2&report=xml
            
            char* requestBuffer = getenv("QUERY_STRING");
            if (requestBuffer)
            {
                String requestString (requestBuffer);
                std::vector<String> delemiters;
                delemiters.push_back("=");
                delemiters.push_back("&");
                
                res = SplitString(requestString, delemiters);
            }
        }
        
        m_parameters = res;
    }

	return res;
}

// Return buffer of graph.
String CGIProcessor::GetGraphBuffer()
{
    String res = m_graphBuffer;
    
    if (res.IsEmpty())
    {
        char * requestMethod = getenv("REQUEST_METHOD");
        if (strstr(requestMethod, "POST"))
        {
            // CGI mode.
            const char * strContentLength = getenv("CONTENT_LENGTH");
            if (strContentLength)
            {
                long contentLength  = strtol(strContentLength, NULL, 10);
                if (contentLength > 0)
                {
                    char* postdata = new char[contentLength + 1];
                    size_t total_readed_size = 0;
                    while (total_readed_size < contentLength)
                    {
                        size_t readed_size = fread(postdata + total_readed_size, 1, contentLength - total_readed_size, stdin);
                        total_readed_size += readed_size;
                        if (readed_size == 0)
                            break;
                    }
                    postdata[contentLength] = 0;
                    res.FromLocale(postdata);
                    delete[] postdata;
                }
            }
        }
        
        m_graphBuffer = res;
    }
    
    return res;
}

std::vector<String> CGIProcessor::SplitString(const String& inputString, const std::vector<String>& delemiters)
{
    std::vector<String> res;
    String input = inputString;
    
    while (!input.IsEmpty())
    {
        int nMinPosition = int(input.Count());
        for (const String& delemiter : delemiters)
        {
            int pos = input.Find(delemiter);
            if (pos >= 0)
            {
                nMinPosition = std::min(pos, nMinPosition);
            }
        }
        
        if (nMinPosition < input.Count())
        {
            String param = String(input).SubStr(0, nMinPosition);
            res.push_back(param);
            input = input.SubStr(nMinPosition + 1);
        }
        else
        {
            break;
        }
    }
    
    if (!input.IsEmpty())
    {
        res.push_back(input);
    }
    return res;
}
