# Compiler
CC=g++
# Flags
CFLAGS=-c -Wall -O3 -I ./algorithm -I ./pugixml -I ./report -I ./common -I ./graph -I . -DCGI_MODE

# Create dirs
MKDIR_P = mkdir -p

all: Dirs GraphOffline

GraphOffline: ./bin/Linux/obj/common/CaseFunctions.o ./bin/Linux/obj/pugixml/pugixml.o ./bin/Linux/obj/GraphOffline.o ./bin/Linux/obj/graph/Graph.o ./bin/Linux/obj/common/ConsoleParams.o ./bin/Linux/obj/common/YString.o ./bin/Linux/obj/CGIProcessor.o ./bin/Linux/obj/algorithm/DijkstraShortPath.o ./bin/Linux/obj/report/GraphMLReporter.o
	$(CC) -fno-use-linker-plugin ./bin/Linux/obj/common/CaseFunctions.o ./bin/Linux/obj/pugixml/pugixml.o ./bin/Linux/obj/GraphOffline.o ./bin/Linux/obj/graph/Graph.o ./bin/Linux/obj/common/ConsoleParams.o ./bin/Linux/obj/common/YString.o ./bin/Linux/obj/CGIProcessor.o ./bin/Linux/obj/algorithm/DijkstraShortPath.o ./bin/Linux/obj/report/GraphMLReporter.o -o ./bin/Linux/Release/GraphOffline

Dirs: ./bin/Linux/obj/common ./bin/Linux/obj/pugixml ./bin/Linux/obj/graph ./bin/Linux/obj/algorithm ./bin/Linux/obj/report ./bin/Linux/Release

./bin/Linux/obj/common:
	${MKDIR_P} ./bin/Linux/obj/common

./bin/Linux/obj/pugixml:
	${MKDIR_P} ./bin/Linux/obj/pugixml

./bin/Linux/obj/graph:
	${MKDIR_P} ./bin/Linux/obj/graph

./bin/Linux/obj/algorithm:
	${MKDIR_P} ./bin/Linux/obj/algorithm

./bin/Linux/obj/report:
	${MKDIR_P} ./bin/Linux/obj/report

./bin/Linux/Release:
	${MKDIR_P} ./bin/Linux/Release

#GraphOffline: ./GraphOffline.o
#	$(CC) -fno-use-linker-plugin GraphOffline.o -o GraphOffline

./bin/Linux/obj/common/CaseFunctions.o: ./common/CaseFunctions.cpp
	$(CC) $(CFLAGS) ./common/CaseFunctions.cpp -o ./bin/Linux/obj/common/CaseFunctions.o

./bin/Linux/obj/pugixml/pugixml.o: ./pugixml/pugixml.cpp
	$(CC) $(CFLAGS) ./pugixml/pugixml.cpp -o ./bin/Linux/obj/pugixml/pugixml.o

./bin/Linux/obj/GraphOffline.o: ./GraphOffline.cpp
	$(CC) $(CFLAGS) ./GraphOffline.cpp -o ./bin/Linux/obj/GraphOffline.o

./bin/Linux/obj/graph/Graph.o: ./graph/Graph.cpp
	$(CC) $(CFLAGS) ./graph/Graph.cpp -o ./bin/Linux/obj/graph/Graph.o

./bin/Linux/obj/common/ConsoleParams.o: ./common/ConsoleParams.cpp
	$(CC) $(CFLAGS) ./common/ConsoleParams.cpp -o ./bin/Linux/obj/common/ConsoleParams.o

./bin/Linux/obj/common/YString.o: ./common/YString.cpp
	$(CC) $(CFLAGS) ./common/YString.cpp -o ./bin/Linux/obj/common/YString.o

./bin/Linux/obj/CGIProcessor.o: ./CGIProcessor.cpp
	$(CC) $(CFLAGS) ./CGIProcessor.cpp -o ./bin/Linux/obj/CGIProcessor.o

./bin/Linux/obj/algorithm/DijkstraShortPath.o: ./algorithm/DijkstraShortPath.cpp
	$(CC) $(CFLAGS) ./algorithm/DijkstraShortPath.cpp -o ./bin/Linux/obj/algorithm/DijkstraShortPath.o

./bin/Linux/obj/report/GraphMLReporter.o: ./report/GraphMLReporter.cpp
	$(CC) $(CFLAGS) ./report/GraphMLReporter.cpp -o ./bin/Linux/obj/report/GraphMLReporter.o

clean:
	rm -rf ./bin/Linux/obj/*
	rm ./bin/Linux/Release/GraphOffline




