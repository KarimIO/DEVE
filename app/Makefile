CXX= c++
CXX_FLAGS= -pedantic -std=c++17 
LINKER_FLAGS = 
EXEC = deve-app

SRC_DIR = src
BUILD_DIR = build

LIB_HEADERS = $(shell find libinclude -name "*.h")
PRECOMPILED_HEADERS = $(addprefix build/, $(patsubst %,%.pch,$(LIB_HEADERS)))

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
	$(CXX) $< $(LIBS) $(CXX_FLAGS) -o $@
#-Iinclude -Ilibinclude 
$(EXEC): $(OBJECTS)
	$(CXX) $(LIBS) $(LINKER_FLAGS) -o $@ $^

simple:
	$(CXX) src/main.cpp -Ilibinclude $(LIBS) -o $(EXEC)

.PHONY: clean

clean:
	rm -rf build/
	rm -rf *.dSYM/
	rm -f $(EXEC)
	rm -f *.out