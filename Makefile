# Das Uber Makefile
#  Utility makefile to actually compile things easily
#  Stick all your cpp files in src then fiddle with the
#  variables below to add includes and libs
#  Set the binary name and voila, that binary will be produced

# BEGIN CUSTOMISATION
BINARY_NAME=cppserver
CXX?=g++
CFLAGS?=-Wall -Werror -g
BOOST_DIR=/usr/local/Cellar/boost/1.47.0
INCLUDE_DIRS=-I$(BOOST_DIR)/include
LIB_DIRS=-L$(BOOST_DIR)/lib
LD_FLAGS=-lboost_system-mt
# END CUSTOMISATION

SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(subst src,build,$(SOURCES:.cpp=.o))

all:
	-mkdir -p build
	$(MAKE) $(BINARY_NAME)

-include $(OBJECTS:.o=.d)

build/%.o: src/%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@
	@$(CXX) -MM $(CFLAGS) -I$(INCLUDE_DIRS) $< > build/$*.d
	@mv -f build/$*.d build/$*.d.tmp
	@sed -e 's|.*:|build/$*.o:|' < build/$*.d.tmp > build/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < build/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> build/$*.d
	@rm -f build/$*.d.tmp

$(BINARY_NAME): $(OBJECTS)
	$(CXX) $(CFLAGS) $(LIB_DIRS) $(LD_FLAGS) $(OBJECTS) -o $@

clean:
	rm -rf build/*
	rm -rf $(BINARY_NAME)