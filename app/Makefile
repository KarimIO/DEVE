CXX= c++
CXX_FLAGS= -pedantic -std=c++17 
LINKER_FLAGS = 
EXEC = deve-app

SRC_DIR = src
BUILD_DIR = build

LIB_INCLUDE_DIR = libinclude
INCLUDE_DIR = include

SUBMODULE_FINAL_LIBRARIES =../submodules/rrad/librrad.so ../submodules/pistache_makefile/libpistache.so
SUBMODULE_INCLUDE_DIRS = $(patsubst %,%include,$(dir $(SUBMODULE_FINAL_LIBRARIES)))
SUBMODULE_LINKS = $(patsubst lib%.so,%,$(notdir $(SUBMODULE_FINAL_LIBRARIES)))

HEADERS = $(shell find include -name "*.h") $(LIB_HEADERS)
SOURCES = $(shell find src -name "*.cpp")
OBJECTS = $(addprefix build/, $(patsubst %.cpp,%.o,$(SOURCES)))

LIBS = -lpthread 

all: debug

debug: CXX_FLAGS += -g
debug: $(EXEC)
release: $(EXEC)

$(SUBMODULE_FINAL_LIBRARIES):
	echo "Building $<..."
	make -C $(@D)

$(OBJECTS): build/%.o : %.cpp $(HEADERS)
	mkdir -p $(@D)
	$(CXX) -I$(INCLUDE_DIR) -I$(LIB_INCLUDE_DIR) $(addprefix -I, $(SUBMODULE_INCLUDE_DIRS)) $(CXX_FLAGS) -c -o $@ $< 

$(EXEC): $(OBJECTS) $(SUBMODULE_FINAL_LIBRARIES)
	cp $(SUBMODULE_FINAL_LIBRARIES) .
	$(CXX)  $(OBJECTS) -L. $(LIBS) $(addprefix -l, $(SUBMODULE_LINKS)) $(LINKER_FLAGS) -o $@

.PHONY: clean

clean:
	rm -rf build/
	rm -f $(EXEC)
	rm -f *.so
