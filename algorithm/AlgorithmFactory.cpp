//
//  AlgorithmFactory.cpp
//  Graphoffline
//
//  Created by Олег on 10.12.15.
//
//

#include "AlgorithmFactory.h"
#include "DijkstraShortPath.h"

std::shared_ptr<IAlgorithm> AlgorithmFactory::CreateAlgorithm(const IGraph* pGraph, const String& name, const ParametersMap& map) const
{
    std::shared_ptr<IAlgorithm> res = m_mAlgorithms.count(name) > 0 ?
        CreateAlgorithm(m_mAlgorithms.at(name), pGraph->GetEdgeWeightType() == WT_FLOAT)
        : std::shared_ptr<IAlgorithm>();
    
    if (res)
    {
        res->SetGraph(pGraph);
        
        IndexType index = 0;
        AlgorithmParam outParamInfo;
        
        while (res->EnumParameter(index, &outParamInfo))
        {
            String paramName = outParamInfo.paramName;
            if (map.count(paramName) > 0)
            {
                switch (outParamInfo.type)
                {
                    case APT_NODE:
                    {
                        ObjectId id = pGraph->GetNode(map.at(outParamInfo.paramName).Locale().Data());
                        if (id != nullptr)
                        {
                            res->SetParameter(outParamInfo.paramName, id);
                        }
                        break;
                    }
                    default : assert(false && "Unknown parameter type");
                }
            }
            
            index++;
        }
    }
    return res;
}
    
const AlgorithmFactory& AlgorithmFactory::GetSingleton()
{
    static AlgorithmFactory factory;
    return factory;
}


AlgorithmFactory::AlgorithmFactory()
{
    IndexType index = 0;
    while (std::shared_ptr<IAlgorithm> algorithm = CreateAlgorithm(index))
    {
        assert(m_mAlgorithms.count(algorithm->GetShortName()) == 0);
        m_mAlgorithms[algorithm->GetShortName()] = index;
        index++;
    }
}

AlgorithmFactory::~AlgorithmFactory()
{

}

std::shared_ptr<IAlgorithm> AlgorithmFactory::CreateAlgorithm(IndexType index, bool bFloat) const
{
    std::shared_ptr<IAlgorithm> res;
    switch (index)
    {
        case 0:
            
            IAlgorithm* pAlgorithm = NULL;
            if (bFloat)
            {
                pAlgorithm = new DijkstraShortPath<IGraphFloat, FloatWeightType>();
            }
            else
            {
                pAlgorithm = new DijkstraShortPath<IGraphInt, IntWeightType>();
            }
            
            res = std::shared_ptr<IAlgorithm>(pAlgorithm);
            break;
    }
 
    return res;
}
