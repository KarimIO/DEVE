CXX= c++
CXX_FLAGS= -pedantic -std=c++17 
CXX_FLAGS_LIB= -fpermissive -std=c++17
LINKER_FLAGS = 
EXEC = deve-app

SRC_DIR = src
BUILD_DIR = build

COMMON_SRC_DIR = ../src
COMMON_INCLUDE_DIR = ../include

LIB_SRC_DIR = ../libsrc
LIB_INCLUDE_DIR = ../libinclude
INCLUDE_DIR = include

SUBMODULE_FINAL_LIBRARIES =../submodules/rrad/librrad.so ../submodules/pistache_makefile/libpistache.so
SUBMODULE_DIRS = $(dir $(SUBMODULE_FINAL_LIBRARIES))
SUBMODULE_INCLUDE_DIRS = $(patsubst %,%include, $(SUBMODULE_DIRS)) $(patsubst %,%libinclude, $(SUBMODULE_DIRS))
SUBMODULE_LINKS = $(patsubst lib%.so,%,$(notdir $(SUBMODULE_FINAL_LIBRARIES)))

LIB_HEADERS = $(shell find $(LIB_INCLUDE_DIR) -name "*.h")
LIB_SOURCES = $(shell find $(LIB_SRC_DIR) -name "*.cpp")
LIB_OBJECTS = $(addprefix build/build/, $(patsubst %.cpp,%.o,$(LIB_SOURCES)))

HEADERS = $(shell find $(INCLUDE_DIR) -name "*.h") $(shell find $(COMMON_INCLUDE_DIR) -name "*.h") $(LIB_HEADERS)
SOURCES = $(shell find $(SRC_DIR) -name "*.cpp") $(shell find $(COMMON_SRC_DIR) -name "*.cpp")
OBJECTS = $(addprefix build/build/, $(patsubst %.cpp,%.o,$(SOURCES)))

LIBS = -pthread 

SUBDIRPATHS = $(shell perl ../split.pl $(SUBMODULE_DIRS) .)

all: debug

debug: CXX_FLAGS += -g
debug: $(EXEC) run
release: $(EXEC) run

.PHONY: clean $(SUBMODULE_FINAL_LIBRARIES)

$(SUBMODULE_FINAL_LIBRARIES):
	+make -C $(@D)

$(LIB_OBJECTS): build/build/%.o : %.cpp $(LIB_HEADERS)
	mkdir -p $(@D)
	$(CXX) -I$(LIB_INCLUDE_DIR) $(CXX_FLAGS_LIB) -c -o $@ $<

$(OBJECTS): build/build/%.o : %.cpp $(HEADERS)
	mkdir -p $(@D)
	$(CXX) -I$(INCLUDE_DIR) -I$(COMMON_INCLUDE_DIR) -I$(LIB_INCLUDE_DIR) $(addprefix -I, $(SUBMODULE_INCLUDE_DIRS)) $(CXX_FLAGS) -c -o $@ $< 

$(EXEC):  $(LIB_OBJECTS) $(OBJECTS) $(SUBMODULE_FINAL_LIBRARIES)
	$(CXX) $(LIBS) $(LINKER_FLAGS) $(OBJECTS) $(LIB_OBJECTS) $(addprefix -l, $(SUBMODULE_LINKS)) $(addprefix -L, $(SUBMODULE_DIRS))  -o $@

run:
	echo '#!/bin/bash' > run
	echo 'LD_LIBRARY_PATH=$(SUBDIRPATHS) ./deve-app $$@' >> run
	chmod +x run

clean:
	rm -rf build/
	rm -f $(EXEC)
	rm -f *.so
	rm -f run
