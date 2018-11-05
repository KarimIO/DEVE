CXX= c++
CXX_FLAGS= -pedantic -std=c++17 
LINKER_FLAGS = 
EXEC = deve-app

SRC_DIR = src
BUILD_DIR = build

LIB_INCLUDE_DIR = libinclude
INCLUDE_DIR = include
SUBMODULE_INCLUDE_DIRS = $(shell find ../submodules -name include)
SUBMODULE_INCLUDES = $(addprefix -I, $(SUBMODULE_INCLUDE_DIRS))

HEADERS = $(shell find include -name "*.h") $(LIB_HEADERS)
SOURCES = $(shell find src -name "*.cpp")
OBJECTS = $(addprefix build/, $(patsubst %.cpp,%.o,$(SOURCES)))

LIBS = -lpistache -lpthread

all: debug

debug: CXX_FLAGS += -g
debug: $(EXEC)
release: $(EXEC)

$(OBJECTS): build/%.o : %.cpp $(HEADERS)
	mkdir -p $(@D)
	$(CXX) $< -I$(INCLUDE_DIR) -I$(LIB_INCLUDE_DIR) $(SUBMODULE_INCLUDES) $(CXX_FLAGS) -o $@

$(EXEC): $(OBJECTS)
	$(CXX) $(LIBS) $(LINKER_FLAGS) -o $@ $^

simple:
	$(CXX) src/main.cpp src/deve_ui_server.cpp src/deve_peer.cpp -Iinclude -Ilibinclude $(LIBS) -o $(EXEC)

.PHONY: clean

clean:
	rm -rf build/
	rm -rf *.dSYM/
	rm -f $(EXEC)
	rm -f *.out