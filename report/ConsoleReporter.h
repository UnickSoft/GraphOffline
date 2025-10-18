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
        uint32_t first_path = 0;
        for (first_path = 0; first_path < pAlgorithm->GetResultCount(); first_path++)
        {
            auto res0 = pAlgorithm->GetResult(first_path);
            if (res0.type == ART_INT)
            {
                if (first_path == 0)
                {
                    sprintf(tempBuffer, "Result is %d", res0.nValue);
                }
                else
                {
                    sprintf(tempBuffer, ", %d", res0.nValue);
                }
            }
            else if (res0.type == ART_FLOAT)
            {
                if (first_path == 0)
                {
                    sprintf(tempBuffer, "Result is %f", res0.fValue);
                }
                else
                {
                    sprintf(tempBuffer, ", %f", res0.fValue);
                }
            }
            else
            {
                break;
            }
            result += String(tempBuffer);
        }

        result += String(" (");

        int pathElementIndex = 0;
        for (int i = first_path; i < pAlgorithm->GetResultCount(); i++)
        {
            AlgorithmResult node = pAlgorithm->GetResult(i);

            if (node.type == ART_NODE_ID)
            {
                if (i != 1)
                {
                    result = result + " ";
                }
                pGraph->GetNodeStrId(node.nodeId, node.strValue, ALGO_RESULT_STRING_SIZE);
                result = result + String(node.strValue);
            }
            else
            {
                if (node.type == ART_SPLIT_PATHS)
                {
                    result = result + " | ";
                    pathElementIndex = 0;
                    continue;
                }

                if (pathElementIndex > 0)
                    result = result + String("->");

                if (node.type == ART_NODES_PATH)
                {
                    result = result + String(node.strValue);
                }
                else if (node.type == ART_EDGES_PATH)
                {
                    result = result + "<" + String(node.strValue) + ">";
                }

                pathElementIndex++;
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
