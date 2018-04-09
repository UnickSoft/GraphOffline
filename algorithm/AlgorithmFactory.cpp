//
//  AlgorithmFactory.cpp
//  Graphoffline
//
//  Created by Олег on 10.12.15.
//
//

#include "AlgorithmFactory.h"
#include "DijkstraShortPath.h"
#include "ConnectedComponent.h"
#include "EulerianPath.h"
#include "MaxFlowPushRelabel.h"

IAlgorithm* AlgorithmFactory::CreateAlgorithm(const char* name, const IGraph* pGraph) const
{
    IAlgorithm* res = m_mAlgorithms.count(String(name)) > 0 ?
        _CreateAlgorithm(m_mAlgorithms.at(String(name)), pGraph->GetEdgeWeightType() == WT_FLOAT)
        : nullptr;
    
    if (res)
    {
        res->SetGraph(pGraph);
        res->SetAlgorithmFactory(this);
    }
    
    return res;
}

std::shared_ptr<IAlgorithm> AlgorithmFactory::CreateAlgorithm(const IGraph* pGraph, const String& name, const ParametersMap& map) const
{
    std::shared_ptr<IAlgorithm> res = m_mAlgorithms.count(name) > 0 ?
        CreateAlgorithm(m_mAlgorithms.at(name), pGraph->GetEdgeWeightType() == WT_FLOAT)
        : std::shared_ptr<IAlgorithm>();
    
    if (res)
    {
        res->SetGraph(pGraph);
        res->SetAlgorithmFactory(this);
        
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
                        if (id != 0)
                        {
                            outParamInfo.data.id = id;
                            res->SetParameter(&outParamInfo);
                        }
                        break;
                    }
                    case APT_FLAG:
                    {
                        bool bFalse = map.at(outParamInfo.paramName) == String("false");
                        bool bTrue  = map.at(outParamInfo.paramName) == String("true");
                        
                        if (bFalse || bTrue)
                        {
                            outParamInfo.data.bFlag = bTrue;
                            res->SetParameter(&outParamInfo);
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
    return std::shared_ptr<IAlgorithm>(_CreateAlgorithm(index, bFloat));
}

IAlgorithm* AlgorithmFactory::_CreateAlgorithm(IndexType index, bool bFloat) const
{
    IAlgorithm* res = nullptr;
    switch (index)
    {
        case 0:
        {
            IAlgorithm* pAlgorithm = nullptr;
            if (bFloat)
            {
                pAlgorithm = new DijkstraShortPath<IGraphFloat, FloatWeightType>();
            }
            else
            {
                pAlgorithm = new DijkstraShortPath<IGraphInt, IntWeightType>();
            }
            
            res = pAlgorithm;
            break;
        }
        case 1:
        {
            res = new EulerianPath(true);//EulerianLoop();
            break;
        }
            
        case 2:
        {
            res = new ConnectedComponent();
            break;
        }
        
        case 3:
        {
            res = new EulerianPath(false);
            break;
        }
        
        case 4:
        {
            IAlgorithm* pAlgorithm = nullptr;
            if (bFloat)
            {
                pAlgorithm = new MaxFlowPushRelabel<IGraphFloat, FloatWeightType>();
            }
            else
            {
                pAlgorithm = new MaxFlowPushRelabel<IGraphInt, IntWeightType>();
            }
            
            res = pAlgorithm;
            break;
        }
    }
 
    return res;
}
