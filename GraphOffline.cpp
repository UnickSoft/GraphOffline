// GraphOffline.cpp : Defines the entry point for the console application.
//

#include <stdio.h> 
#include <stdlib.h> 
#include <memory.h> 

#include "Graph.h"
#include "FileReader.h"
#include "DijkstraShortPath.h"
#include "ConsoleParams.h"
#include "CGIProcessor.h"

int main(int argc, char *argv[]) 
{ 
  int res = 0;
  char * requestMethod = getenv("REQUEST_METHOD");

  if (requestMethod == NULL)
  {
      ConsoleParams consoleParams;
      
      std::vector<String> params;
      for (int i = 1; i < argc; i++)
      {
          params.push_back(argv[i]);
      }
      res = consoleParams.ProcessConsoleParams(params);
      printf("%s\n", consoleParams.GetReport().UTF8().Data());
  }
    
  if (requestMethod)
  {
	printf("Content-Type: text/html\n\n");

  	if (strstr(requestMethod, "POST"))
  	{
		CGIProcessor cgiProcessor;
		res = cgiProcessor.Process();
		printf("%s\n", cgiProcessor.GetReport().UTF8().Data());
    }
  }
  return res;
}
