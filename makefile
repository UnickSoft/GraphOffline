DEBUG ?= 0

# File names
EXEC = ./bin/Linux/Release/GraphOffline
EXEC_TEST = ./bin/Linux/Release/GraphOfflineTest

LIB_DIR = ./lib
ALGORITHM_DIR = algorithm
PUGIXML_DIR = pugixml
REPORT_DIR = report
COMMON_DIR = common
GRAPH_DIR = graph

OBJ_DIR = bin/Linux/obj

# Declaration of variables
CXX ?= g++
CPPFLAGS += -std=c++17 -Wall -I. -I $(ALGORITHM_DIR) -I $(PUGIXML_DIR) -I $(REPORT_DIR) -I $(COMMON_DIR) -I $(GRAPH_DIR) -I $(LIB_DIR) -DCGI_MODE

ifeq ($(DEBUG),1)
	CPPFLAGS += -g
else
	CPPFLAGS += -O3
endif

SOURCES = $(wildcard *.cpp) $(shell find $(ALGORITHM_DIR) $(PUGIXML_DIR) $(REPORT_DIR) $(COMMON_DIR) $(GRAPH_DIR) -name '*.cpp')
HEADERS = $(wildcard *.h) $(shell find $(ALGORITHM_DIR) $(PUGIXML_DIR) $(REPORT_DIR) $(COMMON_DIR) $(GRAPH_DIR) -name '*.h')

# Create obejcts files in $(OBJ_DIR)
OBJECTS = $(patsubst %.cpp,$(OBJ_DIR)/%.o, $(SOURCES))

# Main target
$(EXEC): $(OBJ_DIR)/$(GRAPH_DIR) $(OBJ_DIR)/$(COMMON_DIR) $(OBJ_DIR)/$(REPORT_DIR) $(OBJ_DIR)/$(ALGORITHM_DIR) $(OBJ_DIR)/$(PUGIXML_DIR) $(OBJECTS)
	$(CXX) $(CPPFLAGS) $(OBJECTS) -o $(EXEC)

$(EXEC_TEST): $(OBJ_DIR)/$(GRAPH_DIR) $(OBJ_DIR)/$(COMMON_DIR) $(OBJ_DIR)/$(REPORT_DIR) $(OBJ_DIR)/$(ALGORITHM_DIR) $(OBJ_DIR)/$(PUGIXML_DIR) $(OBJECTS)
	$(CXX) $(CPPFLAGS) $(OBJECTS) -o $(EXEC_TEST)
    
BUILD_TEST: $(EXEC_TEST)
    
# To obtain object files
$(OBJ_DIR)/$(ALGORITHM_DIR)/%.o: $(ALGORITHM_DIR)/%.cpp $(HEADERS)
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(OBJ_DIR)/$(PUGIXML_DIR)/%.o: $(PUGIXML_DIR)/%.cpp $(HEADERS)
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(OBJ_DIR)/$(REPORT_DIR)/%.o: $(REPORT_DIR)/%.cpp $(HEADERS)
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(OBJ_DIR)/$(COMMON_DIR)/%.o: $(COMMON_DIR)/%.cpp $(HEADERS)
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(OBJ_DIR)/$(GRAPH_DIR)/%.o: $(GRAPH_DIR)/%.cpp $(HEADERS)
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.cpp $(HEADERS)
	$(CXX) -c $(CPPFLAGS) $< -o $@

# make dirs
$(OBJ_DIR)/$(ALGORITHM_DIR):
	mkdir -p $(OBJ_DIR)/$(ALGORITHM_DIR)

$(OBJ_DIR)/$(PUGIXML_DIR):
	mkdir -p $(OBJ_DIR)/$(PUGIXML_DIR)

$(OBJ_DIR)/$(REPORT_DIR):
	mkdir -p $(OBJ_DIR)/$(REPORT_DIR)

$(OBJ_DIR)/$(COMMON_DIR):
	mkdir -p $(OBJ_DIR)/$(COMMON_DIR)

$(OBJ_DIR)/$(GRAPH_DIR):
	mkdir -p $(OBJ_DIR)/$(GRAPH_DIR)

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
