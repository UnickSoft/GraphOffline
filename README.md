# GraphOffline

GraphOffline is utility to calculate algorithm on graph (mathematics). This utility used in http://graphonline.ru project.

# License

MIT License.

# How to build

1. Under Windows use VS 2017 or higher. Solution file is GraphOffline.sln.
2. Under Mac OS X use XCode project Graphoffline.xcodeproj.
3. Under Linux/FreeBSD and so on use make file: makefile/makefile-static. makefile-static is used to build with static linking.

# How to use Graphoffline

You may run Graphoofile from command line without args and you see parameters.

## Command line:
```bash
GraphOffline [-debug] ALGORITH_SHORTNAME graphSource [PARAMETERS] [-report reportType]
```
graphSource - GraphML file name. \
-debug - output debug information. By default off. \
-report reportType - setup report type: console or xml. By default console.

Use algorithm short name. Current Available algorithms:
* DijkstraShortPath - shortname is dsp. \
 Parameters: \
  start - start vertex name.\
  finish - finish vertex name.

* Eulerian Loop - shortname is elloop

* Connected component - shortname is concomp \
 Parameters: \
  strong - search weak or strong connected component. [true/false]

* Eulerian Path - shortname is elpath

* MaxFlowPushRelable - shortname is mfpr \
 Parameters: \
  source - source vertex name\
  drain - drain vertex name.

* Hamiltonian Loop - shortname is hamloop

* Hamiltonian Path - shortname is hampath

* Graph Save/Load Test - shortname is loadtest \
  Just test save/load functions.

## Examples:

Search short path from n4 to n7
```bash
 Graphoffline -dsp graph_shortPath.xml -start n4 -finish n7
```

Search strong connected compomemt
```bash
 Graphoffline -concomp graph_shortPath.xml -strong true
```

Get help:
```bash
Graphoffline -help
```

# UnitTests

In directory test we placed UnitTests for algorithms. Under Windows you should run runTests.bat, under Linux "./runTests.sh -linux" and under Mac OS X runTests.sh.

All tests should be - OK.

In any directory inside test you can find example of tests. Usually one test contains graph file in [graphml](https://en.wikipedia.org/wiki/GraphML) format (graph.xml) and corret report for this case (graph.xml.res). Report can be in xml or text format.

Usually when we create new test, we generate report one by one for each case and check output. We try to not change report format to save compatibility with old tests.

# Developer doc

You may find more developer information on our wiki: https://github.com/UnickSoft/GraphOffline/wiki
