.PHONY: all clean cleanProg cleaTests


CPP_FILES = $(wildcard source/*.cpp)
BINARIES = $(wildcard source/*.o)
OUT_NAME = Program

SPDLOG_LIB = external/spdlog/build/libspdlog.a
SPDLOG_INCLUDE = -Iexternal/spdlog/include

CATCH2_SOURCE = external/catch2/catch_amalgamated.cpp 
CATCH2_INCLUDE = -Iexternal

OUT_TEST_NAME = Test
CPP_TEST_FILES = $(wildcard tests/*.cpp)
BINARIES_TEST = $(wildcard tests/*.o)

all: compileProgram compileTests
 
compileProgram: $(CPP_FILES)
	g++ -o $(OUT_NAME) $(CPP_FILES) $(SPDLOG_LIB) $(SPDLOG_INCLUDE)

downloadLogs: 
	cd external && \
	git clone https://github.com/gabime/spdlog.git && \
	cd spdlog && mkdir build && cd build && \
	cmake .. && make -j

compileTests:
	g++ -o $(OUT_TEST_NAME) $(CPP_TEST_FILES) $(CATCH2_SOURCE) $(CATCH2_INCLUDE)

clean: cleanProg cleaTests

cleanProg:
	rm $(BINARIES) $(OUT_NAME)

cleaTests:
	rm $(OUT_TEST_NAME) $(BINARIES_TEST)