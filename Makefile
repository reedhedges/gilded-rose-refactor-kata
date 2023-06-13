
ALL_TARGETS=\
  gilded_basic \
  gilded_basic_optimized \
  gilded_get_update_closures \
  gilded_get_update_functions \
  gilded_wrapper_class \
  gilded_virtual_classes 


all: $(ALL_TARGETS)

clean: 
	-rm $(ALL_TARGETS)

distclean: clean conan-clean

#include conanbuildinfo.mak # Created by conan 'make' generator, defines CONAN_* variables used below
include conandeps.mk
FMT_CXXFLAGS=$(CONAN_INCLUDE_DIRS_FMT:%=-I%) $(CONAN_DEFINES_FMT:%=-D%) $(CONAN_CXXFLAGS_FMT)
FMT_LFLAGS=$(CONAN_LIB_DIRS_FMT:%=-L%) $(CONAN_LIBS_FMT:%=-l%) $(CONAN_SYSTEM_LIBS_FMT:%=-l%)
BENCH_CXXFLAGS=$(CONAN_INCLUDE_DIRS_BENCHMARK:%=-I%) $(CONAN_DEFINES_BENCHMARK:%=-D%) $(CONAN_CXXFLAGS_BENCHMARK)
BENCH_LFLAGS=$(CONAN_LIB_DIRS_BENCHMARK:%=-L%) $(CONAN_LIBS_BENCHMARK:%=-l%) $(CONAN_SYSTEM_LIBS_BENCHMARK:%=-l%)
CATCH2_CXXFLAGS=$(CONAN_INCLUDE_DIRS_CATCH2:%=-I%) $(CONAN_DEFINES_CATCH2:%=-D%) $(CONAN_CXXFLAGS_CATCH2)
CATCH2_LFLAGS=$(CONAN_LIB_DIRS_CATCH2:%=-L%) $(CONAN_LIBS_CATCH2:%=-l%) $(CONAN_SYSTEM_LIBS_CATCH2:%=-l%)

#read_all_conan_args=$(shell cat conanbuildinfo.args) # alternative, from conan compiler args generator

CONAN_TOOLCHAIN_CFLAGS=-m64 -DNDEBUG -D_GLIBCXX_USE_CXX11_ABI=0  # Toolchain flags not generated in conanbuildinfo.mak by conan 'make' generator. (Where is make_toolchain.mak? does it still exist?)

CXX?=g++
CXX+=$(CONAN_TOOLCHAIN_CFLAGS) 

info:
	@echo CXX=$(CXX)
	@echo FMT_CXXFLAGS=$(FMT_CXXFLAGS)
	@echo FMT_LFLAGS=$(FMT_LFLAGS)
	@echo BENCH_CXXFLAGS=$(BENCH_CXXFLAGS)
	@echo BENCH_LFLAGS=$(BENCH_LFLAGS)
	@echo CATCH2_CXXFLAGS=$(CATCH2_CXXFLAGS)
	@echo CATCH2_LFLAGS=$(CATCH2_LFLAGS)

help:
	@echo Targets are: all info help conan clean distclean conan-clean

conandeps.mk &: conanfile.txt
	conan install conanfile.txt && touch $@    # conan doesn't update file timestamp if no new contents were generated (even if conanfile.txt is newer)

conan: conandeps.mk

conan-clean:
	-rm conanbuildinfo.args conanbuildinfo.mak conanbuildinfo.txt conaninfo.txt conan.lock graph_info.json

%: %.cc
	$(CXX) -g -Og -std=c++20 -Wall -Wextra $(FMT_CXXFLAGS) -o $@ $< $(FMT_LFLAGS)

%_benchmark: %.cc
	$(CXX) -g -O3 -std=c++20 -Wall -Wextra -DENABLE_BENCHMARK $(FMT_CXXFLAGS) $(BENCH_CXXFLAGS) -o $@ $< $(FMT_LFLAGS) $(BENCH_LFLAGS)

.PHONY: all conan clean distclean conan-clean help

