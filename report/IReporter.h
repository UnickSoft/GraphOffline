/**
 * Interface to create report.
 *
 */

#pragma once

#include "IAlgorithm.h"
#include "IRelease.h"

class IReporter: public IRelease
{
public:
  // Return report: TODO: return char*
  virtual char* GetReport(IAlgorithmResult* pAlgorithm, IGraph* pGraph) = 0;

  // Free memory of report.
  virtual void FreeReport(const char* report) = 0;
};
