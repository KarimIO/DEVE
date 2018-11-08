CXX= c++
CXX_FLAGS= -pedantic -std=c++17 
LINKER_FLAGS = 
EXEC = deve-ads

SRC_DIR = src
BUILD_DIR = build

COMMON_SRC_DIR = ../src
COMMON_INCLUDE_DIR = ../include

LIB_INCLUDE_DIR = ../libinclude
INCLUDE_DIR =include

OS = $(shell uname)
ifeq ($(OS),Darwin)
	SUBMODULE_FINAL_LIBRARIES=../submodules/rrad/librrad.dylib
else
	SUBMODULE_FINAL_LIBRARIES=../submodules/rrad/librrad.so
endif
SUBMODULE_DIRS = $(dir $(SUBMODULE_FINAL_LIBRARIES))
SUBMODULE_INCLUDE_DIRS =$(patsubst %,%include, $(SUBMODULE_DIRS))
ifeq ($(OS),Darwin)
	SUBMODULE_LINKS =$(patsubst lib%.dylib,%,$(notdir $(SUBMODULE_FINAL_LIBRARIES)))
else
	SUBMODULE_LINKS =$(patsubst lib%.so,%,$(notdir $(SUBMODULE_FINAL_LIBRARIES)))
endif

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

$(OBJECTS): build/build/%.o : %.cpp $(HEADERS)
	mkdir -p $(@D)
	$(CXX) -I$(COMMON_INCLUDE_DIR) -I$(INCLUDE_DIR) -I$(LIB_INCLUDE_DIR) $(addprefix -I, $(SUBMODULE_INCLUDE_DIRS)) $(CXX_FLAGS) -c -o $@ $< 

$(EXEC): $(OBJECTS) $(SUBMODULE_FINAL_LIBRARIES)
	$(CXX) $(LINKER_FLAGS) $(LIBS)  $(OBJECTS) $(addprefix -L, $(SUBMODULE_DIRS)) $(addprefix -l, $(SUBMODULE_LINKS)) -o $@

run:
	echo '#!/bin/bash' > run
	echo 'if [[ `uname` == "Linux" ]]; then' >> run
	echo 'LD_LIBRARY_PATH=$(SUBDIRPATHS) ./deve-ads' >> run
	echo 'elif [[ `uname` == "Darwin" ]]; then' >> run
	echo 'DYLD_LIBRARY_PATH=$(SUBDIRPATHS)  ./deve-ads' >> run
	echo 'fi' >> run
	chmod +x run

clean:
	rm -rf build/
	rm -f $(EXEC)
	rm -f *.so
	rm -f run
