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
#include "CGIProcessor.h"
#include "Logger.h"

#define ALGORITHM_NAME "algorithm"
#define HELP_NAME      "help"

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
        
        if (params.front() == "-" HELP_NAME)
        {
            report = GetHelp();
        }
        else
        {
            if (commands.count(ALGORITHM_NAME) > 0)
            {
                String algorithmShortName = commands.count(ALGORITHM_NAME) > 0 ? commands[ALGORITHM_NAME] : "";
                
                LOG_INFO("Algorith name is " << algorithmShortName.Locale().Data() << ". Graph source is " <<
                         (commands.count(algorithmShortName) > 0 ? commands[algorithmShortName].Locale().Data() : "EMPTY"));
                
                // Graph.
                FloatGraph floatGraph;
                if (commands.count(algorithmShortName) > 0 && LoadGraph(commands[algorithmShortName], floatGraph))
                {
                    IGraph* pGraph = &floatGraph;
                    
                    IntGraph intGraph;
                    if (floatGraph.GetEdgeWeightType() == WT_INT)
                    {
                        if (LoadGraph(commands[algorithmShortName], intGraph))
                        {
                            LOG_INFO("Graph is int");
                            pGraph = &intGraph;
                        }
                    }
                    
                    LOG_INFO(pGraph->PrintGraph());
                    
                    std::shared_ptr<IAlgorithm> algorithm =  AlgorithmFactory::GetSingleton().CreateAlgorithm(pGraph, algorithmShortName, commands);
                    if (algorithm)
                    {
                        res = algorithm->Calculate();
                        ReporterPtr reporter = CreateReporter(commands.count("report") > 0 ? commands["report"] : "");
                        
                        if (reporter)
                        {
                            LOG_INFO("Result is " << algorithm->GetResult(0));
                            
                            uint32_t neededSize = reporter->GetReport(algorithm.get(), pGraph, nullptr, 0);
                            if (neededSize > 0)
                            {
                                char* pBuffer = new char[neededSize];
                                reporter->GetReport(algorithm.get(), pGraph, pBuffer, neededSize);
                                report = pBuffer;
                                delete[] pBuffer;
                            }
                            else
                            {
                                LOG_WARNING("Report is empty");
                            }
                        }
                        else
                        {
                            LOG_ERROR("Cannot create reporter");
                        }
                    }
                    else
                    {
                        LOG_ERROR("Cannot create algorithm");
                    }
                }
            }
            else
            {
                LOG_ERROR("Cannot find algorithm name");
            }
        }
	}
    else
    {
        LOG_WARNING("Param list is empty");
        report = GetHelp();
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

template <class GraphType> bool ConsoleParams::LoadGraph(const String& sourceName, GraphType& graph)
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
    else if (sourceName == "cgiInput")
    {
        String cgiInputXML = m_cgiHelper.GetGraphBuffer();
        BufferChar buffer = cgiInputXML.Locale();
        graph.LoadFromGraphML(buffer.Data(), buffer.Size());
        res = true;
    }
    else
    {
        LOG_ERROR("Cannot load graph");
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


String ConsoleParams::GetHelp()
{
    String res = "GraphOffline utility calculate graph algorithms\n" \
        "Command line:\n" \
        "GraphOffline [-debug] ALGORITH_SHORTNAME graphSource [PARAMETERS] [-report reportType]\n" \
        "graphSource - maybe GraphML file name or cgiInput for input stream.\n"
        "-debug - output debug information.\n" \
        "-report - setup report type. By default console.\n" \
        "\nAvailable alrogithms:\n";

    IndexType index = 0;
    const AlgorithmFactory& algorithmFactory = AlgorithmFactory::GetSingleton();
    std::shared_ptr<IAlgorithm> algorithm;
    
    while (algorithm = algorithmFactory.CreateAlgorithm(index))
    {
        res = res + String(" ") + String(algorithm->GetFullName()) + String(" - shortname is ") + String(algorithm->GetShortName()) +
            String("\n Parameters:\n");
        
        IndexType paramIndex = 0;
        AlgorithmParam outParamInfo;
        while (algorithm->EnumParameter(paramIndex, &outParamInfo))
        {
            res = res + String("  ") + String(outParamInfo.paramName) + String(" \n");
            paramIndex++;
        }
        
        res = res + String("\n");
        
        ++index;
    }
    
    res = res + "\nAvailable reports:\n";
    
    index = 0;
    const ReporterFactory& reporterFactory = ReporterFactory::GetSingleton();
    std::shared_ptr<IReporter> reporter;
    
    while (reporter = reporterFactory.CreateReporter(index))
    {
        res = res + String(" ") + String(reporter->GetFullName()) + String(" - shortname is ") + String(reporter->GetShortName()) + String("\n");
        ++index;
    }
    
    res = res + "\nExamples:\n" \
        " Graphoffline -dsp graph_shortPath.xml -start n4 -finish n7\n" \
        " Graphoffline -help";
    
    return res;
}
