# Standard stuff

.SUFFIXES:

MAKEFLAGS+= --no-builtin-rules  # Disable the built-in implicit rules.
MAKEFLAGS+= --warn-undefined-variables  # Warn when an undefined variable is referenced.

export hostSystemName=$(shell uname)
export CMAKE_EXPORT_COMPILE_COMMANDS=1
export CTEST_OUTPUT_ON_FAILURE=1

CMAKE=${HOME}/.local/bin/cmake # cmake v4.3-rc3

ifeq (${hostSystemName},Darwin)
  export LLVM_PREFIX:=$(shell brew --prefix llvm)
  export LLVM_DIR:=$(shell realpath ${LLVM_PREFIX})
  export PATH:=${LLVM_DIR}/bin:${PATH}
  CMAKE?=/usr/local/bin/cmake

  PRESET?=clang
  STDLIB=libc++
  export CMAKE_CXX_STDLIB_MODULES_JSON:=${LLVM_DIR}/lib/c++/$(STDLIB).modules.json
  #XXX export CXXFLAGS:=-stdlib=$(STDLIB)
  #XXX export LDFLAGS:=-L$(LLVM_DIR)/lib/c++ -lc++abi # XXX -lc++
  export CXX:=clang++
  export GCOV:="llvm-cov gcov"

  ### to test g++-15:
  export GCC_PREFIX:=$(shell brew --prefix gcc)
  export GCC_DIR:=$(shell realpath ${GCC_PREFIX})

  # STDLIB=libstdc++
  # # XXX NOT need! export CMAKE_CXX_STDLIB_MODULES_JSON:=${GCC_DIR}/lib/gcc/current/$(STDLIB).modules.json
  # # XXX NOT need! export CXXFLAGS:=-stdlib=$(STDLIB)
  # export LDFLAGS:=-L$(GCC_DIR)/lib/gcc/current # XXX -lstdc++
  # export CXX:=g++-15
  # export GCOV:="gcov"
else ifeq (${hostSystemName},Linux)
  # TODO(CK): commented out for CI!
  # NOTE: tested with ubuntu:25.04
  # export LLVM_DIR:=/usr/lib/llvm-20
  # export CPPFLAGS:=-I$(LLVM_DIR)/include/c++/v1
  # export CMAKE_CXX_STDLIB_MODULES_JSON:=$(LLVM_DIR)/lib/libc++.modules.json
  # export LDFLAGS:=-L$(LLVM_DIR)/lib/c++ -lc++abi # XXX -lc++
  # export PATH:=${LLVM_DIR}/bin:${PATH}
  # export CXX:=clang++-20

  ### to test g++-15:
  export CXX:=g++-15
  PRESET?=gcc
  CMAKE?=/root/.local/bin/cmake
endif

#############################################################
.PHONY: all test check install clean distclean format workflow
#############################################################

all: build
	ninja -C build all all_verify_interface_header_sets

build: GNUmakefile CMakeLists.txt
	${CMAKE} --version
	-if [ "${hostSystemName}" = "Darwin" ]; then \
		${CXX} -print-file-name=c++/$(STDLIB).modules.json; \
	else \
		${CXX} -print-file-name=$(STDLIB).modules.json; \
	fi
	CXX=${CXX} ${CMAKE} -G Ninja -S . -B build -D CMAKE_BUILD_TYPE=Release \
		-D CMAKE_CXX_STANDARD=26 -D CMAKE_CXX_EXTENSIONS=YES -D CMAKE_CXX_STANDARD_REQUIRED=YES \
		-D CMAKE_CXX_STDLIB_MODULES_JSON=${CMAKE_CXX_STDLIB_MODULES_JSON} \
		-D ASIO_IMPORT_STD=ON --fresh --log-level=VERBOSE -Wdev
	ln -fs build/compile_commands.json .

distclean:
	rm -rf build .cache compile_commands.json
	find . -name .DS_Store -delete
	find . -name '*~' -delete

#############################################################
workflow: CMakePresets.json
	${CMAKE} --preset linux-${PRESET} -D CMAKE_CXX_STANDARD=26 \
		-D ASIO_IMPORT_STD=ON \
		-D ASIO_ENABLE_SSL=ON \
		--fresh --log-level=VERBOSE -Wdev
		# -D CMAKE_CXX_STDLIB_MODULES_JSON=${CMAKE_CXX_STDLIB_MODULES_JSON} \
		#
	${CMAKE} --workflow --preset linux-${PRESET}-release
#############################################################

check: build
	run-clang-tidy src/examples/cpp20

install test: build
	ninja -C build $(@)

format:
	git ls-files ::*.cmake ::*CMakeLists.txt | xargs gersemi -i
	clang-format -i include/asio/detail/std/*.hpp module/asio-gmf.h CMakePresets.json

#############################################################
# Anything we don't know how to build will use this rule.
#############################################################
% ::
	ninja -C build $(@)
