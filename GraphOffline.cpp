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
#include "Logger.h"

// Hardcode debug flag.
bool bDebug = false;

#define EMSCRIPT_DELEMITER "<s\\emscript_split\\s>"

#ifndef EMSCRIPT

int main(int argc, char *argv[])
{
    int res = 0;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-debug") == 0)
        {
            bDebug = true;
        }
    }
    if (bDebug)
    {
        Logger::SetEnableLog(Logger::LE_INFORMATION);
        LOG_INFO("Full log enabled");
    }
    else
    {
        Logger::SetEnableLog(Logger::LE_ERROR);
    }

    char * requestMethod = getenv("REQUEST_METHOD");

    std::vector<String> params;
    ConsoleParams consoleParams;

    if (requestMethod == NULL)
    {
        for (int i = 1 + (bDebug ? 1 : 0); i < argc; i++)
        {
            params.push_back(argv[i]);
        }
    }

    if (requestMethod)
    {
        printf("Content-Type: text/html\n\n");

        if (strstr(requestMethod, "POST"))
        {
            params = CGIProcessor().GetRequestParams();
        }
    }

    {
        LOG_INFO("Input parameters");
        for (String& str : params)
        {
            LOG_INFO(str.Locale().Data());
        }
    }

    res = consoleParams.ProcessConsoleParams(params);
    printf("%s\n", consoleParams.GetReport().UTF8().Data());

    return res;
}

#else

extern "C" {
  const char* ProcessAlgorithm(const char* emscriptParams);
}

std::string res;
const char* ProcessAlgorithm(const char* emscriptParams)
{
  if (strcmp(emscriptParams, EMSCRIPT_DELEMITER) == 0)
  {
    res = "test";
    return res.c_str();
  }

  ConsoleParams consoleParams;
  consoleParams.ProcessConsoleParams(CGIProcessor::SplitString(emscriptParams, { EMSCRIPT_DELEMITER }));
  res = (const char*)consoleParams.GetReport().UTF8().Data();
  return res.c_str();
}

#endif
