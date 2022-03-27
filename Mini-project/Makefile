
APP = bgpsparql
SOURCES = $(wildcard src/*.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
DEPENDS := $(patsubst %.cpp,%.d,$(SOURCES))


CXX = g++
CXXFLAGS = -std=c++11 -Wall -Iinclude 
# -Wextra
.PHONY: all clean


all: $(APP)

bgpsparql: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

-include $(DEPENDS)

%.o: src/%.cpp Makefile
	$(CXX) $(CXXFLAGS) -MD  -c  $< -o $@
clean:
	$(RM) $(OBJECTS) $(DEPENDS) $(APP)
