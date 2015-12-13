/**
 * Interface to create report.
 *
 */

#pragma once

#include "IAlgorithm.h"
#include <memory>

class IReporter
{
public:
    // Return size of report.
    virtual IndexType GetReport(const IAlgorithmResult* pAlgorithm, const IGraph* pGraph,
                               char* buffer, IndexType bufferSize) = 0;
    
    // @return full name.
    virtual const char* GetFullName() = 0;
    
    // @return short name for console param.
    virtual const char* GetShortName() = 0;

    virtual ~IReporter() {}
};

typedef std::shared_ptr<IReporter> ReporterPtr;
