//
//  ReportFactory.cpp
//  Graphoffline
//
//  Created by Олег on 12.12.15.
//
//

#include "ReporterFactory.h"
#include "ConsoleReporter.h"
#include "GraphMLReporter.h"

std::shared_ptr<IReporter> ReporterFactory::CreateReporter(const String& name) const
{
    return m_mReporters.count(name) > 0 ? CreateReporter(m_mReporters.at(name)) : std::shared_ptr<IReporter>();
}

const ReporterFactory& ReporterFactory::GetSingleton()
{
    static ReporterFactory reporterFactory;
    return reporterFactory;
}

ReporterFactory::ReporterFactory()
{
    IndexType index = 0;
    while (std::shared_ptr<IReporter> reporter = CreateReporter(index))
    {
        assert(m_mReporters.count(reporter->GetShortName()) == 0);
        m_mReporters[reporter->GetShortName()] = index;
        index++;
    }
}

ReporterFactory::~ReporterFactory()
{
    
}

std::shared_ptr<IReporter> ReporterFactory::CreateReporter(IndexType index) const
{
    std::shared_ptr<IReporter> res;
    switch (index)
    {
        case 0:
            res = std::shared_ptr<IReporter>(new ConsoleReporter());
            break;
        case 1:
            res = std::shared_ptr<IReporter>(new GraphMLReporter());
            break;
    }
    
    return res;
}

