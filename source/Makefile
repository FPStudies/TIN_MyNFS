.PHONY: clean compile

SOURCE = $(wildcard *.cpp)
BINARIES = $(wildcard *.o)
OUT_NAME = ../Program
FLAGS = -Wall -g -Wextra -pedantic -pthread -std=c++17

SPDLOG_LIB = -l../external/spdlog/build/spdlog
SPDLOG_INCLUDE = -I../external/spdlog/include
INCLUDE = -I../include
INCLUDE_ALL = $(INCLUDE) $(SPDLOG_INCLUDE)

OUT_NAME = ../Program
OUT_LIB_NAME = libsource.a

OBJECT_FILES = $(patsubst %.cpp, %.o, $(SOURCE))
OBJECT_FILES_NO_MAIN = $(filter-out main.o, $(OBJECT_FILES))

MAIN_OBJECT = main.cpp

%.o: %.cpp
	g++ -c $< -o $@ $(FLAGS) $(DEFINE_LOGS) $(INCLUDE_ALL) -L../external/spdlog/build -lspdlog

compile: $(OBJECT_FILES)
	g++ $(FLAGS) $^ $(DEFINE_LOGS) -o $(OUT_NAME) $(INCLUDE_ALL) -L../external/spdlog/build -lspdlog

library: $(OBJECT_FILES_NO_MAIN)
	ar rcs $(OUT_LIB_NAME) $^

clean: 
	rm -f $(BINARIES) $(OUT_NAME)
