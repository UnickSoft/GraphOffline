/**
 * Process console params.
 *
 */

#include "ConsoleParams.h"
#include "GraphMLReporter.h"
#include "ConsoleReporter.h"
#include "YString.h"
#include "FileReader.h"

DijkstraShortPath* ConsoleParams::GetShortPath(const String& filename, const String& source, const String& target)
{
  DijkstraShortPath* res = NULL;
  {
    FileReader file;
    bool bOpen = file.openFile(filename);

    unsigned long fileSize = file.getFileSize();

    if (bOpen && fileSize)
    {
      char* pBuffer = new char[fileSize];
      file.readData(pBuffer, fileSize);

      graph.LoadFromGraphML(pBuffer, fileSize);

      delete[] pBuffer;
      file.closeFile();

      DijkstraShortPath* shortPath = new DijkstraShortPath(graph.FindNode(source), graph.FindNode(target), &graph);
      shortPath->Calculate();

      res = shortPath;
    }
  }

  return res;
}


void ConsoleParams::ParseCommandLine (int argc, char *argv[], ParseCommand& commands)
{
	for (int i = 0; i < argc; i++)
	{
		if (commands.count(argv[i]) > 0 && i < argc - 1)
		{
			commands[argv[i]] = argv[i + 1];
		}
	}
}

// Process:  Find short path: -sp graph_filename -start verate_name -finish vertext_name [-report console|xml]
bool ConsoleParams::FindShortestPathCommand (int argc, char *argv[])
{
  bool res = true;
  ParseCommand commands;
  commands["-start"]  = "";
  commands["-finish"] = "";
  commands["-report"] = "";
  commands["-sp"]     = "";

  ParseCommandLine(argc, argv, commands);

  if (commands["-start"].IsEmpty())
  {
    res = false;
  }
  if (commands["-finish"].IsEmpty())
  {
    res = false;
  }
  if (commands["-sp"].IsEmpty())
  {
    res = false;
  }
  if (commands["-report"].IsEmpty())
  {
    commands["-report"] = "console";
  }

  if (res)
  {                                                                            
    IReporter* pReporter    = CreateReporter(commands["-report"]);

    DijkstraShortPath* path = GetShortPath(commands["-sp"], commands["-start"], commands["-finish"]);

    report = pReporter->GetReport(path, &graph);

    pReporter->Release();

    delete path;
  }

  return res;
}

bool ConsoleParams::ProcessConsoleParams(int argc, char *argv[])
{
	bool res = false;
	report.Clear();

	// Find shortest path.
	if (argc > 1 && strcmp(argv[1], "-sp") == 0)
	{
		res = FindShortestPathCommand (argc, argv);
	}

	return res;
}

IReporter* ConsoleParams::CreateReporter(const String& reporterName)
{
	IReporter* res = NULL;

	if (reporterName == "console")
	{
		res = new ConsoleReporter();
	}
	else if (reporterName == "xml")
	{
		res = new GraphMLReporter();
	}

	return res;
}

String ConsoleParams::GetReport()
{
	return report;
}