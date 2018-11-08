CXX= c++
CXX_FLAGS= -pedantic -std=c++17 
LINKER_FLAGS = 
EXEC = deve-app

SRC_DIR = src
BUILD_DIR = build

COMMON_SRC_DIR = ../src
COMMON_INCLUDE_DIR = ../include

LIB_INCLUDE_DIR = ../libinclude
INCLUDE_DIR = include

SUBMODULE_FINAL_LIBRARIES =../submodules/rrad/librrad.so ../submodules/pistache_makefile/libpistache.so
SUBMODULE_DIRS = $(dir $(SUBMODULE_FINAL_LIBRARIES))
SUBMODULE_INCLUDE_DIRS = $(patsubst %,%include, $(SUBMODULE_DIRS))
SUBMODULE_LINKS = $(patsubst lib%.so,%,$(notdir $(SUBMODULE_FINAL_LIBRARIES)))

HEADERS = $(shell find $(INCLUDE_DIR) -name "*.h") $(shell find $(COMMON_INCLUDE_DIR) -name "*.h") $(LIB_HEADERS)
SOURCES = $(shell find $(SRC_DIR) -name "*.cpp") $(shell find $(COMMON_SRC_DIR) -name "*.cpp")
OBJECTS = $(addprefix build/build/, $(patsubst %.cpp,%.o,$(SOURCES)))

LIBS = -pthread 

all: debug

debug: CXX_FLAGS += -g
debug: $(EXEC) run
release: $(EXEC) run

$(SUBMODULE_FINAL_LIBRARIES):
	echo "Building $<..."
	make -C $(@D)

$(OBJECTS): build/build/%.o : %.cpp $(HEADERS)
	mkdir -p $(@D)
	$(CXX) -I$(INCLUDE_DIR) -I$(COMMON_INCLUDE_DIR) -I$(LIB_INCLUDE_DIR) $(addprefix -I, $(SUBMODULE_INCLUDE_DIRS)) $(CXX_FLAGS) -c -o $@ $< 

$(EXEC): $(OBJECTS) $(SUBMODULE_FINAL_LIBRARIES)
	cp $(SUBMODULE_FINAL_LIBRARIES) .
	$(CXX)  $(OBJECTS) $(addprefix -L, $(SUBMODULE_DIRS)) $(LIBS) $(addprefix -l, $(SUBMODULE_LINKS)) $(LINKER_FLAGS) -o $@

run:
	echo '#!/bin/bash' > run
	echo 'LD_LIBRARY_PATH=$(shell perl ../split.pl $(SUBMODULE_DIRS)) ./deve-app' >> run
	chmod +x run

.PHONY: clean

clean:
	rm -rf build/
	rm -f $(EXEC)
	rm -f *.so
	rm -f run
