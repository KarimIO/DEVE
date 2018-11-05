INCLUDE_DIR = ../pistache/include
HEADERS = $(shell find ../pistache/include -name "*.h") $(LIB_HEADERS)
SOURCES = $(shell find ../pistache/src | grep .cc)
OBJECTS = $(addprefix build/build/, $(patsubst %.cc,%.o,$(SOURCES)))

COMPILER_FLAGS = -fPIC
LINKER_FLAGS = -lpthread -shared

LIB = libpistache.so
all: $(LIB)


$(OBJECTS): build/build/%.o : %.cc $(HEADERS)
	echo $(OBJECTS)
	mkdir -p $(@D)
	c++ $(COMPILER_FLAGS) -I$(INCLUDE_DIR) -c -o $@ $< 

$(LIB): $(OBJECTS)
	@echo "\nBuilding final binary..."
	c++ $(LINKER_FLAGS) -o $(LIB) $^
	@echo "Build complete."

.PHONY: clean

clean:
	rm -rf build
	rm -f $(LIB)
