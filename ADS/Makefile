CC = cc
CXX = c++
FLAGS = -pedantic -std=c11
CXX_FLAGS = -pedantic -std=c++17
LINKER_FLAGS = 
EXEC = deve-ads

INCLUDE_DIR =include
SRC_DIR = src
BUILD_DIR = build/$(shell uname)/ADS

COMMON_SRC_DIR = ../src
COMMON_INCLUDE_DIR = ../include

LIB_INCLUDE_DIR = ../libinclude
LIB_SRC_DIR = ../libsrc

OS = $(shell uname)
ifeq ($(OS),Darwin)
	SUBMODULE_FINAL_LIBRARIES=../submodules/rrad/librrad.dylib
else
	SUBMODULE_FINAL_LIBRARIES=../submodules/rrad/librrad.so
endif
SUBMODULE_DIRS = $(dir $(SUBMODULE_FINAL_LIBRARIES))
SUBMODULE_INCLUDE_DIRS =$(patsubst %,%include, $(SUBMODULE_DIRS))  $(patsubst %,%libinclude, $(SUBMODULE_DIRS))
ifeq ($(OS),Darwin)
	SUBMODULE_LINKS =$(patsubst lib%.dylib,%,$(notdir $(SUBMODULE_FINAL_LIBRARIES)))
else
	SUBMODULE_LINKS =$(patsubst lib%.so,%,$(notdir $(SUBMODULE_FINAL_LIBRARIES)))
endif

LIB_HEADERS = $(shell find $(LIB_INCLUDE_DIR) -name "*.h")
LIB_SOURCES = $(shell find $(LIB_SRC_DIR) -name "*.c")
LIB_OBJECTS = $(addprefix $(BUILD_DIR)/, $(patsubst %.c,%.o,$(LIB_SOURCES)))
LIB_CPP_SOURCES = $(shell find $(LIB_SRC_DIR) -name "*.cpp")
LIB_CPP_OBJECTS = $(addprefix $(BUILD_DIR)/, $(patsubst %.cpp,%.o,$(LIB_CPP_SOURCES)))

HEADERS = $(shell find $(INCLUDE_DIR) -name "*.h") $(shell find $(COMMON_INCLUDE_DIR) -name "*.h") $(LIB_HEADERS)
SOURCES = $(shell find $(SRC_DIR) -name "*.cpp") $(shell find $(COMMON_SRC_DIR) -name "*.cpp")
OBJECTS = $(addprefix $(BUILD_DIR)/, $(patsubst %.cpp,%.o,$(SOURCES)))
LIBS = -pthread

SUBDIRPATHS = $(shell perl ../split.pl $(SUBMODULE_DIRS) .)

all: debug

debug: CXX_FLAGS += -g
debug: CXX_FLAGS_LIB += -g
debug: $(EXEC) run
release: $(EXEC) run

.PHONY: clean $(SUBMODULE_FINAL_LIBRARIES)

$(SUBMODULE_FINAL_LIBRARIES):
	+make -C $(@D)

$(LIB_OBJECTS): $(BUILD_DIR)/%.o : %.c $(LIB_HEADERS)
	mkdir -p $(@D)
	$(CC) -I$(LIB_INCLUDE_DIR) $(FLAGS) -c -o $@ $<

$(LIB_CPP_OBJECTS): $(BUILD_DIR)/%.o : %.cpp $(LIB_HEADERS)
	mkdir -p $(@D)
	$(CXX) -I$(LIB_INCLUDE_DIR) $(CXX_FLAGS) -c -o $@ $<

$(OBJECTS): $(BUILD_DIR)/%.o : %.cpp $(HEADERS)
	mkdir -p $(@D)
	$(CXX) -I$(COMMON_INCLUDE_DIR) -I$(INCLUDE_DIR) -I$(LIB_INCLUDE_DIR) $(addprefix -I, $(SUBMODULE_INCLUDE_DIRS)) $(CXX_FLAGS) -c -o $@ $< 

$(EXEC): $(OBJECTS) $(LIB_OBJECTS) $(LIB_CPP_OBJECTS) $(SUBMODULE_FINAL_LIBRARIES)
	$(CXX) $(LINKER_FLAGS) $(LIBS)  $(OBJECTS) $(LIB_OBJECTS)  $(LIB_CPP_OBJECTS) $(addprefix -L, $(SUBMODULE_DIRS)) $(addprefix -l, $(SUBMODULE_LINKS)) -o $@

run:
	echo '#!/bin/bash' > run
	echo 'if [[ `uname` == "Linux" ]]; then' >> run
	echo 'LD_LIBRARY_PATH=$(SUBDIRPATHS) ./deve-ads' >> run
	echo 'elif [[ `uname` == "Darwin" ]]; then' >> run
	echo 'DYLD_LIBRARY_PATH=$(SUBDIRPATHS)  ./deve-ads' >> run
	echo 'fi' >> run
	chmod +x run
	echo '#!/bin/bash' > llrun
	echo 'if [[ `uname` == "Linux" ]]; then' >> llrun
	echo 'LD_LIBRARY_PATH=$(SUBDIRPATHS) lldb ./deve-ads' >> llrun
	echo 'elif [[ `uname` == "Darwin" ]]; then' >> llrun
	echo 'DYLD_LIBRARY_PATH=$(SUBDIRPATHS)  lldb ./deve-ads' >> llrun
	echo 'fi' >> llrun
	chmod +x llrun

clean:
	rm -rf build/
	rm -f $(EXEC)
	rm -f *.so
	rm -f run
	rm -f llrun
