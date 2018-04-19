
#include "IAlgorithm.h"
#include <string>
#include "GraphMLReporter.h"
#include <stdio.h>
#include <cstring>
#include <map>

const char* xmlStart = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n \
    <graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"\n \
        xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n \
        xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns\n \
         http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n";

const char* xmlStartShort = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n \
    <graphml>\n";

const char* xmlEnd = "</graphml>\n";

const char* xmlGraphHeaderMask = "<graph id=\"G\" edgedefault=\"undirected\">\n";
const char* xmlGraphFooter = "</graph>\n";

const char* xmlGraphNodeStart = "<node id=\"%s\">\n";
const char* xmlGraphNodeEnd = "</node>\n";

const char* xmlGraphEdge = "<edge source=\"%s\" target=\"%s\"/>\n";
const char* xmlGraphEdgeStart = "<edge source=\"%s\" target=\"%s\">\n";
const char* xmlGraphEdgeEnd   = "</edge>\n";

const char* xmlGraphNodePropertyInt   = "<data key=\"%s\">%d</data>\n";
const char* xmlGraphNodePropertyFloat = "<data key=\"%s\">%f</data>\n";

const char* xmlGraphNodeHightlight     = "<data key=\"hightlightNode\">%d</data>\n";

const char* xmlResultHead   = "<result count=\"%d\">\n";
const char* xmlResultFooter = "</result>\n";
const char* xmlResultValueHead = "  <value type=\"%d\">";
const char* xmlResultValueFooter = "</value>\n";


#define MAX_NODE_CHAR 128
#define MAX_ID 32

IndexType GraphMLReporter::GetReport(const IAlgorithmResult* pAlgorithm, const IGraph* pGraph,
                                     char* buffer, IndexType bufferSize)
{
    if (pGraph->GetEdgeWeightType() == WT_INT)
    {
        return GetReport<IntWeightType>(pAlgorithm, pGraph, buffer, bufferSize);
    }
    else
    {
        return GetReport<FloatWeightType>(pAlgorithm, pGraph, buffer, bufferSize);
    }
}

template <typename WeightType> IndexType GraphMLReporter::GetReport(const IAlgorithmResult* pAlgorithm, const IGraph* pGraph, char* buffer, IndexType bufferSize)
{
    IndexType res = 0;
    if (pAlgorithm && pGraph)
    {
        auto getPropertyMask = [](const AlgorithmResultType& type) -> const char*
        {
        if (type == ART_INT)
        {
            return xmlGraphNodePropertyInt;
        }
        else
        {
            return xmlGraphNodePropertyFloat;
        }
        };
        

        std::string result = xmlStartShort;
        result += xmlGraphHeaderMask;
        

        char strBuffer[MAX_NODE_CHAR] = {0};
        sprintf(strBuffer, xmlResultHead, pAlgorithm->GetResultCount());
        result += strBuffer;
        
        // Add result.
        for (IndexType i = 0; i < pAlgorithm->GetResultCount(); i++)
        {
            AlgorithmResult res = pAlgorithm->GetResult(i);
            
            sprintf(strBuffer, xmlResultValueHead, res.type);
            
            result += strBuffer;
            
            switch (res.type) {
                case ART_INT:
                    sprintf(strBuffer, "%d", res.nValue);
                    break;
                case ART_FLOAT:
                    sprintf(strBuffer, "%f", res.fValue);
                    break;
                case ART_STRING:
                    sprintf(strBuffer, "%s", res.strValue);
                    break;
                case ART_NODES_PATH:
                    sprintf(strBuffer, "%s", res.strValue);
                    break;
                default:
                    strBuffer[0] = 0;
                    break;
            }
            result += strBuffer;
            
            result += xmlResultValueFooter;
        }
        
        result += xmlResultFooter;
        
        // Add nodes. <node>
        std::map<ObjectId, bool> hightlightNodes;
        for (int i = 0; i < pAlgorithm->GetHightlightNodesCount(); i++)
        {
            hightlightNodes[pAlgorithm->GetHightlightNode(i)] = true;
        }
    
        for (int i = 0; i < pGraph->GetNodesCount(); i++)
        {
            char* strNodeStrId[MAX_ID] = {0};
            pGraph->GetNodeStrId(pGraph->GetNode(i), (char *)strNodeStrId, MAX_ID);
            
            char graphNode[MAX_NODE_CHAR]  = {0};
            sprintf(graphNode, xmlGraphNodeStart, strNodeStrId);
            result += graphNode;
            // Low dist
            IndexType index = 0;
            AlgorithmResult property;
            while (pAlgorithm->GetNodeProperty(pGraph->GetNode(i), index, &property) && property.type != ART_UNKNOWN)
            {
                sprintf(graphNode, getPropertyMask(property.type), pAlgorithm->GetNodePropertyName(index), (WeightType)(property.type == ART_INT ? property.nValue : property.fValue));
                result += graphNode;
                index++;
            }
            
            
            // Hightlight or not
            sprintf(graphNode, xmlGraphNodeHightlight, (hightlightNodes.count(pGraph->GetNode(i)) > 0));
            result += graphNode;
            
            result += xmlGraphNodeEnd;
        }
        
        
        // Add edges. <edge>
        for (int i = 0; i < pAlgorithm->GetHightlightEdgesCount(); i++)
        {
            NodesEdge edge = pAlgorithm->GetHightlightEdge(i);
            char* strSourceNodeId[MAX_ID] = {0};
            char* strTargetNodeId[MAX_ID] = {0};
            
            if (pGraph->IsEgdeExists(edge.source, edge.target))
            {
                pGraph->GetNodeStrId(edge.source, (char *)strSourceNodeId, MAX_ID);
                pGraph->GetNodeStrId(edge.target, (char *)strTargetNodeId, MAX_ID);
            }
            else
            {
                pGraph->GetNodeStrId(edge.target, (char *)strSourceNodeId, MAX_ID);
                pGraph->GetNodeStrId(edge.source, (char *)strTargetNodeId, MAX_ID);
            }
            
            AlgorithmResult property;
            if (pAlgorithm->GetEdgeProperty(edge, 0, &property) && property.type != ART_UNKNOWN)
            {
                char graphEdge[MAX_NODE_CHAR]  = {0};
                sprintf(graphEdge, xmlGraphEdgeStart, strSourceNodeId, strTargetNodeId);
                result += graphEdge;
                
                // Low dist
                IndexType index = 0;
                while (pAlgorithm->GetEdgeProperty(edge, index, &property) && property.type != ART_UNKNOWN)
                {
                    if (property.type == ART_INT)
                    {
                        sprintf(graphEdge, getPropertyMask(property.type), pAlgorithm->GetEdgePropertyName(index), property.nValue);
                    }
                    else
                    {
                        sprintf(graphEdge, getPropertyMask(property.type), pAlgorithm->GetEdgePropertyName(index), property.fValue);
                    }
                    result += graphEdge;
                    index++;
                }
                
                result += xmlGraphEdgeEnd;
            }
            else
            {
                char graphEdge[MAX_NODE_CHAR]  = {0};
                sprintf(graphEdge, xmlGraphEdge, strSourceNodeId, strTargetNodeId);
                result += graphEdge;
            }
        }
        
        result += xmlGraphFooter;
        result += xmlEnd;
        
        res = result.length() + 1;
        
        if (bufferSize >= res)
        {
            strncpy(buffer, result.c_str(), result.length());
            buffer[result.length()] = '\0';
        }
    }
    
    return res;
}

