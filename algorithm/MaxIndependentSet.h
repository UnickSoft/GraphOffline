//
//  MaxIndependentSet.h
//  Graphoffline
//
//  Created by Abdalla Egbareia on 16/04/2025
//
//

#ifndef Max_Independent_Set_HPP
#define Max_Independent_Set_HPP

#include <stdio.h>
#include "BaseAlgorithm.h"
#include "IGraph.h"
#include <vector>

class MaxIndependentSet : public BaseAlgorithm
{
public:
    MaxIndependentSet ();
    virtual ~MaxIndependentSet ();

    // Long name of algoright: MaxIndependentSet.
    virtual const char* GetFullName() const override {return "Max Independent Set";};
    // Short name of algorithm: mis
    virtual const char* GetShortName() const override {return "mis"; };
    // Calculate algorithm.
    virtual bool Calculate() override ;

    // Get result count.
    virtual IndexType GetResultCount() const override;
    // Get result of index. Algorithms can have complex result.
    virtual AlgorithmResult GetResult(IndexType index) const override;

    virtual void UnitTest() const override {}

private:

    std::vector<ObjectId> m_maxIndependentSet;

};

#endif /* Max_Independent_Set_HPP */
