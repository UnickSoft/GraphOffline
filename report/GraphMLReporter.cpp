
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

const char* xmlGraphHeaderMaskInt = "<graph id=\"G\" edgedefault=\"undirected\" result=\"%d\">\n";
const char* xmlGraphHeaderMaskFloat = "<graph id=\"G\" edgedefault=\"undirected\" result=\"%f\">\n";

const char* xmlGraphFooter = "</graph>\n";
const char* xmlGraphNodeStart = "<node id=\"%s\">\n";
const char* xmlGraphNodeEnd = "</node>\n";
const char* xmlGraphEdge = "<edge source=\"%s\" target=\"%s\"/>\n";

const char* xmlGraphNodePropertyInt   = "<data key=\"%s\">%d</data>\n";
const char* xmlGraphNodePropertyFloat = "<data key=\"%s\">%f</data>\n";

const char* xmlGraphNodeHightlight     = "<data key=\"hightlightNode\">%d</data>\n";


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
        const char* xmlGraphHeaderMask = NULL;
        const char* xmlGraphNodeProperty = NULL;
        
        if (pGraph->GetEdgeWeightType() == WT_INT)
        {
            xmlGraphHeaderMask = xmlGraphHeaderMaskInt;
            xmlGraphNodeProperty = xmlGraphNodePropertyInt;
        }
        else
        {
            xmlGraphHeaderMask = xmlGraphHeaderMaskFloat;
            xmlGraphNodeProperty = xmlGraphNodePropertyFloat;
        }
        
        std::string result = xmlStartShort;
        char graphHeader[MAX_NODE_CHAR]  = {0};
        AlgorithmResult algorithResult =  pAlgorithm->GetResult();
        sprintf(graphHeader, xmlGraphHeaderMask, (WeightType)(algorithResult.type == ART_INT ? algorithResult.nValue : algorithResult.fValue));
        result += graphHeader;
        
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
            while (pAlgorithm->GetProperty(pGraph->GetNode(i), index, &property) && property.type != ART_UNKNOWN)
            {
                sprintf(graphNode, xmlGraphNodeProperty, pAlgorithm->GetPropertyName(index), (WeightType)(property.type == ART_INT ? property.nValue : property.fValue));
                result += graphNode;
                index++;
            }
            
            
            // Hightlight or not
            sprintf(graphNode, xmlGraphNodeHightlight, (hightlightNodes.count(pGraph->GetNode(i)) > 0));
            result += graphNode;
            
            result += xmlGraphNodeEnd;
        }
        
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
            
            char graphEdge[MAX_NODE_CHAR]  = {0};
            sprintf(graphEdge, xmlGraphEdge, strSourceNodeId, strTargetNodeId);
            
            result += graphEdge;
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

