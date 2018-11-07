CXX= c++
CXX_FLAGS= -pedantic -std=c++17 
LINKER_FLAGS = 
EXEC = deve-ads

SRC_DIR = src
BUILD_DIR = build

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

HEADERS = $(shell find include -name "*.h") $(LIB_HEADERS)
SOURCES = $(shell find src -name "*.cpp")
OBJECTS = $(addprefix build/, $(patsubst %.cpp,%.o,$(SOURCES)))

LIBS = -lpthread 

all: debug

debug: CXX_FLAGS += -g
debug: $(EXEC) run
release: $(EXEC) run

$(SUBMODULE_FINAL_LIBRARIES):
	echo "Building $<..."
	make -C $(@D)

$(OBJECTS): build/%.o : %.cpp $(HEADERS)
	mkdir -p $(@D)
	$(CXX) -I$(INCLUDE_DIR) -I$(LIB_INCLUDE_DIR) $(addprefix -I, $(SUBMODULE_INCLUDE_DIRS)) $(CXX_FLAGS) -c -o $@ $< 

$(EXEC): $(OBJECTS) $(SUBMODULE_FINAL_LIBRARIES)
	$(CXX)  $(OBJECTS) $(addprefix -L, $(SUBMODULE_DIRS)) $(LIBS) $(addprefix -l, $(SUBMODULE_LINKS)) $(LINKER_FLAGS) -o $@

run:
	echo '#!/bin/sh' > run
	echo 'if [[ `uname` == "Linux" ]]; then' >> run
	echo 'LD_LIBRARY_PATH=$(shell perl ../split.pl $(SUBMODULE_DIRS)) ./deve-ads' >> run
	echo 'elif [[ `uname` == "Darwin" ]]; then' >> run
	echo 'DYLD_LIBRARY_PATH=$(shell perl ../split.pl $(SUBMODULE_DIRS)) ./deve-ads' >> run
	echo 'fi' >> run
	chmod +x run

.PHONY: clean

clean:
	rm -rf build/
	rm -f $(EXEC)
	rm -f *.so
	rm -f run
