//
//  ConnectedСomponent.hpp
//  Graphoffline
//
//  Created by Олег on 27.12.15.
//
//

#ifndef Connected_Component_hpp
#define Connected_Component_hpp

#include <stdio.h>

#include <stdio.h>
#include "IAlgorithm.h"
#include "IGraph.h"

class ConnectedComponent : public BaseAlgorithm
{
public:
    ConnectedComponent ();
    virtual ~ConnectedComponent ();
    
    // Long name of algoright: DijkstraShortPath.
    virtual const char* GetFullName() const  override {return "Connected component";};
    // Short name of algorithm: dsp
    virtual const char* GetShortName() const  override {return "concomp";};
    // Enum parameters
    virtual bool EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const override;
    // Set parameter to algorithm.
    virtual void SetParameter(const AlgorithmParam* outParamInfo) override;
    // Calculate algorithm.
    virtual bool Calculate() override;
    // Get result count.
    virtual IndexType GetResultCount() const override;
    // Get result of index. Algorithms can have complex result.
    virtual AlgorithmResult GetResult(IndexType index) const override;
    
protected:
    
    bool FindWeakComponent();
    bool FindStrongComponent();
    
    IntWeightType m_nConnectedCompCount;
    bool m_bStrongComponent;
};

#endif /* Connected_Component_hpp */
