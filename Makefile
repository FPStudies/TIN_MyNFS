.PHONY: all clean cleanProg cleanTests compile downloadLogs compileNoLogs compileProgramNoLogs compileTestNoLogs library libraryNoLogs

#/**
#* @author Mateusz Kordowski
#*/

CPP_FILES = $(wildcard source/*.cpp)
BINARIES = $(wildcard source/*.o)
OUT_NAME = Program
FLAGS = -Wall -g

SPDLOG_LIB = external/spdlog/build/libspdlog.a
SPDLOG_INCLUDE = -Iexternal/spdlog/include
INCLUDE = -Iinclude

CATCH2_SOURCE = external/catch2/catch_amalgamated.cpp 
CATCH2_INCLUDE = -Iexternal

OUT_TEST_NAME = Test
CPP_TEST_FILES = $(wildcard tests/*.cpp)
BINARIES_TEST = $(wildcard tests/*.o)

DEFINE_LOGS = -DENABLE_LOGS

all: compile

downloadLogs: 
	cd external && \
	git clone https://github.com/gabime/spdlog.git && \
	cd spdlog && mkdir build && cd build && \
	cmake .. && make -j

library:
	cd source && $(MAKE) library DEFINE_LOGS=-DENABLE_LOGS

libraryNoLogs:
	cd source && $(MAKE) library

compile: compileProgram compileTest

compileNoLogs: compileProgramNoLogs compileTestNoLogs

compileProgramNoLogs:
	cd source && $(MAKE) compile

compileTestNoLogs:
	cd tests && $(MAKE) compile

compileProgram:
	cd source && $(MAKE) compile DEFINE_LOGS=-DENABLE_LOGS

compileTest:
	cd tests && $(MAKE) compile DEFINE_LOGS=-DENABLE_LOGS

clean: cleanProg cleanTests

cleanProg:
	cd source && $(MAKE) clean

cleanTests:
	cd tests && $(MAKE) clean
