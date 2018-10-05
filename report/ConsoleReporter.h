/**
 * Class return report for console.
 *
 * Result = 4 (0 -> 2 -> 6 -> 8 -> 0).
 */

#pragma once

#include "IReporter.h"
#include <string>
#include <stdio.h>
#include <string.h>
#include "YString.h"

#define MAX_REPORT 1024
#define MAX_ID 32


class ConsoleReporter : public IReporter
{
public:

    /**
     * @return report in GraphML format.
     */
    virtual IndexType GetReport(const IAlgorithmResult* pAlgorithm, const IGraph* pGraph,
                                char* buffer, IndexType bufferSize)
    {
        String result;
        
        char tempBuffer[MAX_REPORT] = {0};
        if (pGraph->GetEdgeWeightType() == WT_INT)
        {
            sprintf(tempBuffer, "Result is %d (", pAlgorithm->GetResult(0).nValue);
        }
        else
        {
            sprintf(tempBuffer, "Result is %f (", pAlgorithm->GetResult(0).fValue);
        }
        result = String(tempBuffer);
        
        for (int i = 1; i < pAlgorithm->GetResultCount(); i++)
        {
            AlgorithmResult node = pAlgorithm->GetResult(i);
            
            if (node.type == ART_NODES_PATH)
            {
                result = result + String (node.strValue) + String(i < pAlgorithm->GetResultCount() - 1 ? "->" : "");
            }
        }
        
        result = result + String(")");
        
        Buffer8 bufferUTF = result.UTF8();
        
        uint32_t outBufferSize = uint32_t(bufferUTF.Size() + 1);
        
        if (outBufferSize <= bufferSize)
        {
            memcpy(buffer, bufferUTF.Data(), bufferUTF.Size());
            buffer[bufferUTF.Size()] = 0;
        }
        
        return outBufferSize;
    }
    
    // @return full name.
    virtual const char* GetFullName() { return "Console Reporter"; }
    
    // @return short name for console param.
    virtual const char* GetShortName() { return "console"; }

};
