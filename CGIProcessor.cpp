#include "CGIProcessor.h"
#include "DijkstraShortPath.h"
#include "GraphMLReporter.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <memory.h> 

CGIProcessor::CGIProcessor(void)
{
}

CGIProcessor::~CGIProcessor(void)
{
}

bool CGIProcessor::Process()
{
	bool res = false;

	report.Clear();

	char * requestMethod = getenv("REQUEST_METHOD");
	if (strstr(requestMethod, "POST"))
	{
		// CGI mode.
		char * strContentLength = getenv("CONTENT_LENGTH");
		long contentLength  = strtol(strContentLength, NULL, 10);

		// "REQUEST_STRING" should have n0->n2 data or n0->n2|Debug
		char* requestBuffer = getenv("QUERY_STRING");
		String requestString (requestBuffer);
		int debugPosition = requestString.Find("DebugOutput");
		bool bDebugOutput = false;
		bool bDebugFile = false;
		if (debugPosition >= 0)
		{
			bDebugOutput = true;
			requestString = requestString.SubStr(0, debugPosition);
		}

		if (bDebugOutput)
		{
			printf("contentLength = %d\n", contentLength);
		}
		
		// We process xml from 0 to 1 Mb.
		if (contentLength > 0)
		{
			char* postdata = new char[contentLength + 1];
			memset(postdata, 0, contentLength + 1);
			if (!postdata)
			{ 
				printf ("Error!!!");
				exit(EXIT_FAILURE);
			}
			//fgets(postdata, contentLength + 1, stdin);
			fread(postdata, contentLength + 1, 1, stdin);
			if (bDebugOutput)
			{
				printf("Output xml: %s\n", postdata);
			}

			Graph graph;
			if (graph.LoadFromGraphML(postdata, contentLength + 1))
			{
				if (bDebugOutput)
				{
					printf("Request string: %s\n", requestString.Locale().Data());
				}

				int position = requestString.Find("to");
				if (position >= 0)
				{
					String sourceId = String(requestString).SubStr(0, position);
					String targetId = String(requestString).SubStr(position + 2);
					if (bDebugOutput)
					{
						printf("Find from %s to %s\n",
							sourceId.Locale().Data(), targetId.Locale().Data());
					}

					DijkstraShortPath shortPath(graph.FindNode(sourceId), graph.FindNode(targetId), &graph);
					shortPath.Calculate();

					GraphMLReporter reporter;
					const char* reportBuffer = reporter.GetReport(&shortPath, &graph);

					report = String(reportBuffer);

					if (bDebugFile)
					{
						FILE* file = fopen("GraphCGI.log", "w");
						if (file)
						{
							fprintf(file, reportBuffer);
							fclose(file);
						}
					}

					reporter.FreeReport(reportBuffer);
					res = true;
				}

				delete[] postdata;
			}
		}    
	}

	return res;
}

// Return report after process.
String CGIProcessor::GetReport()
{
	return report;
}
