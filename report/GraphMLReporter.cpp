
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

const char* xmlGraphHeaderMask = "<graph id=\"G\" edgedefault=\"undirected\" result=\"%d\">\n";

const char* xmlGraphFooter = "</graph>\n";
const char* xmlGraphNodeStart = "<node id=\"%s\">\n";
const char* xmlGraphNodeEnd = "</node>\n";
const char* xmlGraphEdge = "<edge source=\"%s\" target=\"%s\"/>\n";
const char* xmlGraphNodeLowestDistance = "<data key=\"lowestDistance\">%d</data>\n";
const char* xmlGraphNodeHightlight     = "<data key=\"hightlightNode\">%d</data>\n";


#define MAX_NODE_CHAR 128
#define MAX_ID 32
      
char* GraphMLReporter::GetReport(IAlgorithmResult* pAlgorithm, IGraph* pGraph)
{
  char* resultBuffer = NULL;
  if (pAlgorithm && pGraph)
  {
    std::string result = xmlStartShort;
    char graphHeader[MAX_NODE_CHAR]  = {0};
    sprintf(graphHeader, xmlGraphHeaderMask, pAlgorithm->GetResult());
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
		sprintf(graphNode, xmlGraphNodeLowestDistance, pAlgorithm->GetProperty(pGraph->GetNode(i), "lowestDistance"));
		result += graphNode;

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

      if (pGraph->IsEgdeExistsInInput(edge.source, edge.target))
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

    resultBuffer = new char[result.length() + 1];
    memset(resultBuffer, 0, result.length() + 1);
    strncpy(resultBuffer, result.c_str(), result.length());
  }

  return resultBuffer;
}


// Free memory.
void GraphMLReporter::FreeReport(const char* report)
{
  delete[] report;
}