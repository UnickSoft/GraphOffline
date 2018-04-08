//
//  AlgorithmFactory.hpp
//  Graphoffline
//
//  Created by Олег on 10.12.15.
//
//

#ifndef AlgorithmFactory_hpp
#define AlgorithmFactory_hpp

#include <stdio.h>
#include <map>
#include <memory>
#include "IAlgorithm.h"
#include "YString.h"
#include "IAlgorithmFactory.h"

// Map of parameter name and value.
typedef std::map<String, String> ParametersMap;

class AlgorithmFactory : public IAlgorithmFactory
{
public:
    
    // IAlgorithmFactory
    // Create algorithm for given Graph.
    virtual IAlgorithm* CreateAlgorithm(const char* name, const IGraph* pGraph) const override;
    
    std::shared_ptr<IAlgorithm> CreateAlgorithm(const IGraph* pGraph, const String& name, const ParametersMap& map) const;
    
    static const AlgorithmFactory& GetSingleton();
    
    std::shared_ptr<IAlgorithm> CreateAlgorithm(IndexType index, bool bFloat = true) const;
    
protected:
    
    IAlgorithm* _CreateAlgorithm(IndexType index, bool bFloat = true) const;
    
    AlgorithmFactory();
    ~AlgorithmFactory();
    
    // Map of algorithms.
    std::map<String, IndexType> m_mAlgorithms;
    
};

#endif /* AlgorithmFactory_hpp */
