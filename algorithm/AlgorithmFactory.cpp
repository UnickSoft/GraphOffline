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
#include "HamiltonianLoop.h"
#include "GraphLoadTest.h"
#include "Logger.h"
#include "WeightMultiGraph.h"
#include "IsomorphismCheck.h"
#include "PrintAllPaths.h"
#include "BellmanFord.h"
#include "MaxClique.h"

#include <iostream>

static std::vector<std::string> ParseNodeList(const std::string & packData)
{
  std::vector<std::string> res;

  std::string line;
  std::stringstream ss(packData);

  while (std::getline(ss, line, ','))
  {
    res.push_back(line);
  }

  return res;
}

struct NodePairStr
{
  std::string source;
  std::string target;
};

static std::vector<NodePairStr> ParseEdgeList(const std::string & packData)
{
  std::vector<NodePairStr> res;

  std::string line;
  std::stringstream ss(packData);

  while (std::getline(ss, line, ','))
  {
    NodePairStr edge;
    std::stringstream edgess(line);
    std::getline(edgess, edge.source, '-');
    std::getline(edgess, edge.target, '-');
    res.push_back(edge);
  }

  return res;
}

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
        res->SetAlgorithmFactory(this);
        res->SetGraph(pGraph);
        if (pGraph->IsMultiGraph() && !res->IsSupportMultiGraph())
        {
            LOG_INFO("Algorithm does not support multigraph");
            return nullptr;
        }

        IndexType index = 0;
        AlgorithmParam outParamInfo;

        try {
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
                    case APT_NODE_LIST:
                    {
                        auto nodesList = ParseNodeList(map.at(outParamInfo.paramName).Locale().Data());

                        if (nodesList.empty())
                            break;

                        outParamInfo.data.ids = new ObjectId[nodesList.size() + 1];
                        outParamInfo.data.ids[nodesList.size()] = std::numeric_limits<ObjectId>::max();
                        int index = 0;
                        for (auto & node : nodesList)
                        {
                            ObjectId id = pGraph->GetNode(node.c_str());
                            if (id != 0)
                            {
                                outParamInfo.data.ids[index] = id;
                                index++;
                            }
                        }

                        if (index == nodesList.size())
                            res->SetParameter(&outParamInfo);

                        delete[] outParamInfo.data.ids;
                        break;
                    }
                    case APT_EDGE_LIST:
                    {
                        auto edgeList = ParseEdgeList(map.at(outParamInfo.paramName).Locale().Data());

                        if (edgeList.empty())
                            break;

                        outParamInfo.data.ids = new ObjectId[edgeList.size() + 1];
                        outParamInfo.data.ids[edgeList.size()] = std::numeric_limits<ObjectId>::max();
                        int index = 0;
                        for (auto & edge : edgeList)
                        {
                            ObjectId source = pGraph->GetNode(edge.source.c_str());
                            ObjectId target = pGraph->GetNode(edge.target.c_str());
                            ObjectId id = pGraph->GetEdge(source, target);
                            if (id != 0)
                            {
                                outParamInfo.data.ids[index] = id;
                                index++;
                            }
                        }

                        if (index == edgeList.size())
                            res->SetParameter(&outParamInfo);

                        delete[] outParamInfo.data.ids;
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
                    case APT_NUMBER:
                    {
                        // const char *str_num =
                        //      map.at(outParamInfo.paramName).Locale().Data();

#define LOCAL_BUFF_SIZE 11
                        char str_num[LOCAL_BUFF_SIZE + 1];
                        String param = map.at(outParamInfo.paramName);
                        int i = 0;
                        while (i < param.Count() && i < LOCAL_BUFF_SIZE) {
                          str_num[i] = param.GetAt(i);
                          ++i;
                        }
                        str_num[i] = '\0';
                        i = 0;
                        while (str_num[i] != '\0' && i < LOCAL_BUFF_SIZE)
                        {
                            assert(std::isdigit(str_num[i]));
                            i++;
                        }
                        assert(str_num[i] == '\0');

                        outParamInfo.data.val = std::atoi(str_num);
                        res->SetParameter(&outParamInfo);
                        break;
                    }

                    case APT_STRING:
                    {
                        std::strncpy(outParamInfo.data.str, map.at(outParamInfo.paramName).Locale().Data(), ALGO_PARAM_STRING_SIZE);
                        res->SetParameter(&outParamInfo);
                        break;
                    }

                    default : assert(false && "Unknown parameter type");
                    }
                }

                index++;
            }
        }
        catch (const std::invalid_argument &e)
        {
            LOG_ERROR(e.what());
            return nullptr;
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

IMultiGraph* AlgorithmFactory::CreateMultiGraph(const IGraph* pGraph) const
{
    if (pGraph->GetEdgeWeightType() == WT_INT)
    {
        return dynamic_cast<IMultiGraph*>(IntMultiGraph::CreateGraph(pGraph));
    }
    else
    {
        return dynamic_cast<IMultiGraph*>(FloatMultiGraph::CreateGraph(pGraph));
    }
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

        case 5:
        {
            res = new HamiltonianLoop(false);
            break;
        }

        case 6:
        {
            res = new HamiltonianLoop(true);
            break;
        }

        case 7:
        {
            res = new GraphLoadTest();
            break;
        }

        case 8:
        {
          res = new IsomorphismCheck();
          break;
        }

        case 9:
        {
            res = new PrintAllPaths();
            break;
        }

        case 10:
        {
            IAlgorithm* pAlgorithm = nullptr;
            if (bFloat)
            {
                pAlgorithm = new BellmanFord<IGraphFloat, FloatWeightType>();
            }
            else
            {
                pAlgorithm = new BellmanFord<IGraphInt, IntWeightType>();
            }

            res = pAlgorithm;
            break;
        }

       case 11:
       {
           res = new MaxClique();
           break;
       }

    }

    return res;
}
