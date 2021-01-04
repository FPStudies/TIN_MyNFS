.PHONY: clean compile


SOURCE = $(wildcard *.cpp)
BINARIES = $(wildcard *.o)
FLAGS = -Wall -g -Wextra -pedantic -pthread -std=c++17

INCLUDE = -I../include

CATCH2_SOURCE = ../external/catch2/catch_amalgamated.cpp 
CATCH2_INCLUDE = -I../external

OUT_NAME = ../Test

OBJECT_FILES = $(patsubst %.cpp, %.o, $(SOURCE))
OBJECT_EXTERNAL = $(patsubst %.cpp, %.o, $(CATCH2_SOURCE))


%.o: %.cpp
	g++ -c $< -o $@ $(FLAGS) $(DEFINE_LOGS) $(INCLUDE) $(CATCH2_INCLUDE)

$(OBJECT_EXTERNAL): $(CATCH2_SOURCE)
	g++ -c $< -o $@ $(FLAGS)

compile: $(OBJECT_FILES) $(OBJECT_EXTERNAL)
	g++ $(FLAGS) $^ $(DEFINE_LOGS) -o $(OUT_NAME)

clean: 
	rm -f $(BINARIES) $(OUT_NAME)
