# Standard stuff

.SUFFIXES:

MAKEFLAGS+= --no-builtin-rules  # Disable the built-in implicit rules.
MAKEFLAGS+= --warn-undefined-variables  # Warn when an undefined variable is referenced.

export hostSystemName=$(shell uname)
export CTEST_OUTPUT_ON_FAILURE=1

ifeq (${hostSystemName},Darwin)
  export LLVM_PREFIX:=$(shell brew --prefix llvm)
  export LLVM_DIR:=$(shell realpath ${LLVM_PREFIX})
  export PATH:=${LLVM_DIR}/bin:${PATH}
  #XXX CMAKE=${HOME}/.local/bin/cmake # cmake v4.3-rc2
  CMAKE?=/usr/local/bin/cmake

  STDLIB=libc++
  export CMAKE_CXX_STDLIB_MODULES_JSON:=${LLVM_DIR}/lib/c++/$(STDLIB).modules.json
  export CXXFLAGS:=-stdlib=$(STDLIB)
  export LDFLAGS:=-L$(LLVM_DIR)/lib/c++ -lc++abi # XXX -lc++
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
  # export LLVM_DIR:=/usr/lib/llvm-22
  # export PATH:=${LLVM_DIR}/bin:${PATH}
  # export CXX:=clang++22
  CMAKE=cmake
endif

#############################################################
.PHONY: all test check install clean format
#############################################################

all: build
	ninja -C build all all_verify_interface_header_sets

build: GNUmakefile CMakeLists.txt
	${CMAKE} --version
	${CXX} -print-file-name=$(STDLIB).modules.json
	CXX=${CXX} ${CMAKE} -G Ninja -S . -B build -D CMAKE_CXX_STANDARD=26 -D CMAKE_BUILD_TYPE=Release \
		-D CMAKE_CXX_STDLIB_MODULES_JSON=${CMAKE_CXX_STDLIB_MODULES_JSON} \
		-D ASIO_IMPORT_STD=ON --fresh --log-level=VERBOSE -Wdev
	ln -fs build/compile_commands.json .

clean:
	rm -rf build .cache compile_commands.json
	find . -name .DS_Store -delete
	find . -name '*~' -delete

check: build
	run-clang-tidy src/examples/cpp20

install test: build
	ninja -C build $(@)

format:
	git ls-files ::*.cmake ::*CMakeLists.txt | xargs gersemi -i

#############################################################
# Anything we don't know how to build will use this rule.
#############################################################
% ::
	ninja -C build $(@)
