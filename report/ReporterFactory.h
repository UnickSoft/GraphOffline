//
//  ReportFactory.hpp
//  Graphoffline
//
//  Created by Олег on 12.12.15.
//
//

#ifndef ReportFactory_hpp
#define ReportFactory_hpp

#include <stdio.h>
#include <memory>
#include "YString.h"
#include "IReporter.h"
#include <map>

class ReporterFactory
{
public:
    
    // Create reported by name.
    std::shared_ptr<IReporter> CreateReporter(const String& name) const;
    
    static const ReporterFactory& GetSingleton();

    std::shared_ptr<IReporter> CreateReporter(IndexType index) const;
    
private:
    
    ReporterFactory();
    ~ReporterFactory();
    
    // Map of reporters.
    std::map<String, IndexType> m_mReporters;
    
};

#endif /* ReportFactory_hpp */
