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
  virtual char* GetReport(IAlgorithmResult* pAlgorithm, IGraph* pGraph)
  {
    String result;

    char buffer[MAX_REPORT] = {0};
    sprintf(buffer, "Result is %d (", pAlgorithm->GetResult());
    result = String(buffer);

    for (int i = 0; i < pAlgorithm->GetHightlightEdgesCount(); i++)
    {
      NodesEdge edge = pAlgorithm->GetHightlightEdge(i);
	  char* strSourceNodeId[MAX_ID] = {0};
	  pGraph->GetNodeStrId(edge.source, (char *)strSourceNodeId, MAX_ID);
      char* strTargetNodeId[MAX_ID] = {0};
	  pGraph->GetNodeStrId(edge.target, (char *)strTargetNodeId, MAX_ID);

      result = result + String(i > 0 ? "," : "") + String ((char *)strSourceNodeId) + String("->") + String((char *)strTargetNodeId);
    }

    result = result + String(")");

	Buffer8 bufferUTF = result.UTF8();
	char* pBufferResult = new char[bufferUTF.Size() + 1];
	memcpy(pBufferResult, bufferUTF.Data(), bufferUTF.Size());
	pBufferResult[bufferUTF.Size()] = 0;

    return pBufferResult;
  }

  // Free memory.
  virtual void FreeReport(const char* report)
  {
    delete[] report;
  }

  // Release object.
  virtual void Release()
  {
	  delete this;
  }

};