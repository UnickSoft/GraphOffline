//
//  IAlgorithmFactory.h
//  Graphoffline
//
//  Created by Олег on 06.01.16.
//
//

#ifndef IAlgorithmFactory_h
#define IAlgorithmFactory_h

#include "IAlgorithm.h"

class IAlgorithmFactory
{
public:
    
    // Create algorithm for given Graph.
    virtual IAlgorithm* CreateAlgorithm(const char* name, const IGraph* pGraph) const = 0;
    
    // Create MultipleGraph from common.
    virtual IMultiGraph* CreateMultiGraph(const IGraph* pGraph) const = 0;
    
    virtual ~IAlgorithmFactory() {}
};


#endif /* IAlgorithmFactory_h */
