cmake_minimum_required(VERSION 3.16)

project(
  Graphoffline
  VERSION 1.0
  LANGUAGES CXX)

add_executable(GraphOffline)
add_executable(GraphCGI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
  target_compile_options(GraphOffline PUBLIC -Wall -std=c++17 -Werror -Wno-defaulted-function-deleted -Wno-range-loop-construct -Wno-unused-function)
  target_compile_options(GraphCGI PUBLIC -Wall -std=c++17 -Werror -Wno-defaulted-function-deleted -Wno-range-loop-construct  -Wno-unused-function)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(GraphOffline PUBLIC -Wall -std=c++17)
  target_compile_options(GraphCGI PUBLIC -Wall -std=c++17)
else()
  target_compile_options(GraphOffline PUBLIC -Wall /std:c++17)
  target_compile_options(GraphCGI PUBLIC -Wall /std:c++17)
endif()


add_definitions(-DRAPIDJSON_HAS_STDSTRING)

target_compile_definitions(GraphOffline
    PUBLIC
    UNICODE
    _UNICODE
)

target_compile_definitions(GraphCGI
    PUBLIC
    CGI_MODE
    UNICODE
    _UNICODE
)

set(COMMON_LIB_SOURCES 
    CGIProcessor.cpp
    CGIProcessor.h
    IAlgorithmFactory.h
    WeightMultiGraph.h
    algorithm/AlgorithmFactory.cpp
    algorithm/AlgorithmFactory.h
    algorithm/BaseAlgorithm.cpp
    algorithm/BaseAlgorithm.h
    algorithm/BellmanFord.h
    algorithm/BellmanFordImpl.h
    algorithm/ConnectedComponent.cpp
    algorithm/ConnectedComponent.h
    algorithm/DijkstraShortPath.h
    algorithm/DijkstraShortPathImpl.h
    algorithm/EulerianPath.cpp
    algorithm/EulerianPath.h
    algorithm/GraphLoadTest.cpp
    algorithm/GraphLoadTest.h
    algorithm/HamiltonianLoop.cpp
    algorithm/HamiltonianLoop.h
    algorithm/IAlgorithm.h
    algorithm/IsomorphismCheck.cpp
    algorithm/IsomorphismCheck.h
    algorithm/MaxClique.cpp
    algorithm/MaxClique.h
    algorithm/MaxFlowPushRelabel.h
    algorithm/MaxFlowPushRelableImpl.h
    algorithm/PrintAllPaths.cpp
    algorithm/PrintAllPaths.h
    algorithm/ShortestPath.h
    bin/emscripten/DebugTest.html
    bin/emscripten/ReleaseTest.html
    common/Buffer.h
    common/CaseFunctions.cpp
    common/CaseFunctions.h
    common/ConsoleParams.cpp
    common/ConsoleParams.h
    common/FileReader.h
    common/Logger.cpp
    common/Logger.h
    common/RefCounter.h
    common/Utils.h
    common/YString.cpp
    common/YString.h
    graph/Graph.cpp
    graph/Graph.h
    graph/IGraph.h
    graph/MultiGraph.cpp
    graph/MultiGraph.h
    graph/WeightGraph.h
    lib/CBioInfCpp.h
    pugixml/foreach.hpp
    pugixml/pugiconfig.hpp
    pugixml/pugixml.cpp
    pugixml/pugixml.hpp
    report/ConsoleReporter.h
    report/GraphMLReporter.cpp
    report/GraphMLReporter.h
    report/IReporter.h
    report/ReporterFactory.cpp
    report/ReporterFactory.h
)

add_library(Common STATIC 
  ${COMMON_LIB_SOURCES}
  )

add_library(CommonCGI STATIC 
  ${COMMON_LIB_SOURCES}
  )

target_include_directories(Common PUBLIC
    .
    lib
    algorithm
    pugixml
    report
    common
    graph
  )

target_include_directories(CommonCGI PUBLIC
    .
    lib
    algorithm
    pugixml
    report
    common
    graph
  )

target_compile_definitions(Common PRIVATE 
  UNICODE
  _UNICODE
  )

target_compile_definitions(CommonCGI PRIVATE 
  CGI_MODE
  UNICODE
  _UNICODE
  )

target_include_directories(GraphOffline PUBLIC
    .
    lib
    algorithm
    pugixml
    report
    common
    graph
  )

target_include_directories(GraphCGI PUBLIC
    .
    lib
    algorithm
    pugixml
    report
    common
    graph
  )

target_sources(GraphOffline 
                PRIVATE
                GraphOffline.cpp
                )

target_sources(GraphCGI 
                PRIVATE
                GraphOffline.cpp
                )

if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
  target_link_libraries(GraphOffline Common)
  target_link_libraries(GraphCGI CommonCGI)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(GraphOffline Common -lstdc++fs)
  target_link_libraries(GraphCGI CommonCGI -ldl -lpthread -lstdc++fs)
else()
  target_link_libraries(GraphOffline Common -lstdc++fs)
  target_link_libraries(GraphCGI CommonCGI -ldl -lpthread -lstdc++fs)
endif()

# enable testing functionality
#enable_testing()

# define tests
#add_test(
#  NAME Sim
#  COMMAND $<TARGET_FILE:Sim> -test
#)