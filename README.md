# GraphOffline

GraphOffline is utility to calculate algorithm on graph (mathematics). This utility used in http://graphonline.ru project.

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
  start \
  finish

* Eulerian Loop - shortname is elloop \
 Parameters:

* Connected component - shortname is concomp \
 Parameters: \
  strong 

* Eulerian Path - shortname is elpath \
 Parameters:

* MaxFlowPushRelable - shortname is mfpr \
 Parameters: \
  source \
  drain

* Hamiltonian Loop - shortname is hamloop \
 Parameters:

* Hamiltonian Path - shortname is hampath \
 Parameters:

* Graph Save/Load Test - shortname is loadtest \
 Parameters:

## Examples:
```bash
 Graphoffline -dsp graph_shortPath.xml -start n4 -finish n7
```
```bash
Graphoffline -help
```

# UnitTests

