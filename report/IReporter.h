/**
 * Interface to create report.
 *
 */

#pragma once

#include "IAlgorithm.h"

class IReporter
{
public:
    // Return size of report.
    virtual IndexType GetReport(const IAlgorithmResult* pAlgorithm, const IGraph* pGraph,
                               char* buffer, IndexType bufferSize) = 0;

    virtual ~IReporter() {}
};
