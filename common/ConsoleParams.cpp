/**
 * Process console params.
 *
 */

#include "ConsoleParams.h"
#include "GraphMLReporter.h"
#include "ConsoleReporter.h"
#include "YString.h"
#include "FileReader.h"
#include "ReporterFactory.h"
#include "AlgorithmFactory.h"

#define ALGORITHM_NAME "algorithm"

void ConsoleParams::ParseCommandLine (const std::vector<String>& params, ParametersMap& commands)
{
    if (!params.empty())
    {
        commands[ALGORITHM_NAME] = GetRealParamName(params.front());
        
        for (auto param = params.begin(); param < params.end(); param += 2)
        {
            if ((param + 1) != params.end())
            {
                commands[GetRealParamName(*param)] = *(param + 1);
            }
        }
    }
}

bool ConsoleParams::ProcessConsoleParams(const std::vector<String>& params)
{
	bool res = false;
	report.Clear();

	// Find shortest path.
	if (!params.empty())
	{
        ParametersMap commands;
        ParseCommandLine(params, commands);
        
        if (commands.count(ALGORITHM_NAME) > 0)
        {
            String algorithmShortName = commands.count(ALGORITHM_NAME) > 0 ? commands[ALGORITHM_NAME] : "";
            
            // Graph.
            Graph graph;
            if (commands.count(algorithmShortName) > 0 && LoadGraph(commands[algorithmShortName], graph))
            {
                std::shared_ptr<IAlgorithm> algorithm =  AlgorithmFactory::GetSingleton().CreateAlgorithm(&graph, algorithmShortName, commands);
                if (algorithm)
                {
                    res = algorithm->Calculate();
                    ReporterPtr reporter = CreateReporter(commands.count("report") > 0 ? commands["report"] : "");
                    
                    if (reporter)
                    {
                        uint32_t neededSize = reporter->GetReport(algorithm.get(), &graph, nullptr, 0);
                        if (neededSize > 0)
                        {
                            char* pBuffer = new char[neededSize];
                            reporter->GetReport(algorithm.get(), &graph, pBuffer, neededSize);
                            report = pBuffer;
                            delete[] pBuffer;
                        }
                    }
                }
            }
        }
	}

	return res;
}

std::shared_ptr<IReporter> ConsoleParams::CreateReporter(const String& reporterName)
{
    return ReporterFactory::GetSingleton().CreateReporter(reporterName.IsEmpty() ? "console" : reporterName);
}

String ConsoleParams::GetReport()
{
	return report;
}

bool ConsoleParams::LoadGraph(const String& sourceName, Graph& graph)
{
    FileReader file;
    
    bool res = false;
    
    if (file.openFile(sourceName) && file.getFileSize() > 0)
    {
        unsigned long fileSize = file.getFileSize();
        char* pBuffer = new char[fileSize];
        file.readData(pBuffer, fileSize);
        
        graph.LoadFromGraphML(pBuffer, fileSize);
        
        delete[] pBuffer;
        file.closeFile();
        res = true;
    }

    return res;
}


String ConsoleParams::GetRealParamName(const String& paramName)
{
    String res = paramName;
    if (res.GetAt(0) == Char32('-'))
    {
        res = res.SubStr(1);
    }
    return res;
}
