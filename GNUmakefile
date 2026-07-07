# Standard stuff

.SUFFIXES:

MAKEFLAGS+= --no-builtin-rules  # Disable the built-in implicit rules.
MAKEFLAGS+= --warn-undefined-variables  # Warn when an undefined variable is referenced.

# TODO: export CMAKE_CXX_COMPILER_LAUNCHER=ccache
# TODO: export CMAKE_C_COMPILER_LAUNCHER=ccache
export CMAKE_CONFIG_TYPE=Release
export CMAKE_CONFIGURATION_TYPES="Release;Debug"
export CMAKE_EXPORT_COMPILE_COMMANDS=YES
export CMAKE_GENERATOR=Ninja
# XXX export CMAKE_INSTALL_PREFIX="${HOME}/.local"
# XXX export CMAKE_PREFIX_PATH="${HOME}/.local"

# NOTE: only to use experimental cmake versions:
# TODO: export PATH="${HOME}/.local/bin:${PATH}"

export hostSystemName:=$(shell uname)

ifeq (${hostSystemName},Darwin)

  ### NOTE: to test clang++-22:
  ifeq (${CXX},clang++)
    STDLIB:=libc++
    PRESET:=clang
    SYSROOT:=$(shell xcrun --show-sdk-path)
    export LLVM_PREFIX:=$(shell brew --prefix llvm)
    export LLVM_DIR:=$(shell realpath ${LLVM_PREFIX})
    export PATH:=${LLVM_DIR}/bin:${PATH}
    export CMAKE_CXX_STDLIB_MODULES_JSON:=${LLVM_DIR}/lib/c++/$(STDLIB).modules.json
    export CXXFLAGS:=-stdlib=$(STDLIB) --sysroot=$(SYSROOT)
    export LDFLAGS:=-L$(LLVM_DIR)/lib/c++ # XXX -lc++abi
    # XXX export CXX:=clang++
    # XXX export GCOV:="llvm-cov gcov"
  endif

  ### NOTE: to test g++-16:
  ifeq (${CXX},g++-16)
    STDLIB:=libstdc++
    PRESET:=gcc
    export GCC_PREFIX:=$(shell brew --prefix gcc)
    export GCC_DIR:=$(shell realpath ${GCC_PREFIX})
    export CMAKE_CXX_STDLIB_MODULES_JSON:=${GCC_DIR}/lib/gcc/current/$(STDLIB).modules.json
    export CXXFLAGS:=-stdlib=$(STDLIB)
    # XXX export CXX:=g++-16
    # XXX export GCOV:="gcov"
  endif

else ifeq (${hostSystemName},Linux)
  ifeq (${CXX},clang++)
    STDLIB:=libstdc++
    PRESET:=clang
    export LLVM_DIR:=/usr/lib/llvm-23
    export PATH:=${LLVM_DIR}/bin:${PATH}
    export CXX:=clang++-23
  endif
endif

#############################################################
.PHONY: all test check install clean distclean format workflow
#############################################################

all: build
	ninja -C build all all_verify_interface_header_sets

build: GNUmakefile CMakeLists.txt
	cmake --version
	-if [ "${hostSystemName}" = "Darwin" ]; then \
		${CXX} -print-file-name=c++/$(STDLIB).modules.json; \
	else \
		${CXX} -print-file-name=$(STDLIB).modules.json; \
	fi
	CXX=${CXX} cmake -G Ninja -S . -B build -D CMAKE_BUILD_TYPE=Release \
		-D CMAKE_CXX_STANDARD=26 -D CMAKE_CXX_EXTENSIONS=NO -D CMAKE_CXX_STANDARD_REQUIRED=YES \
		-D CMAKE_CXX_STDLIB_MODULES_JSON=${CMAKE_CXX_STDLIB_MODULES_JSON} \
		-D CMAKE_SKIP_TEST_ALL_DEPENDENCY=NO \
		-D ASIO_IMPORT_STD=ON --fresh --log-level=VERBOSE -Wdev
	ln -fs build/compile_commands.json .

distclean:
	rm -rf build .cache compile_commands.json
	find . -name .DS_Store -delete
	find . -name '*~' -delete

#############################################################
workflow: CMakePresets.json
	cmake --preset linux-${PRESET} -D CMAKE_CXX_STANDARD=26 \
		-D ASIO_IMPORT_STD=ON \
		-D ASIO_ENABLE_SSL=ON \
		--fresh --log-level=VERBOSE -Wdev
		# -D CMAKE_CXX_STDLIB_MODULES_JSON=${CMAKE_CXX_STDLIB_MODULES_JSON} \
		#
	cmake --workflow --preset linux-${PRESET}-release
#############################################################

check: build
	run-clang-tidy src/examples/cpp20

install test: build
	ninja -C build $(@)

format:
	git ls-files ::*.cmake ::*CMakeLists.txt | xargs gersemi -i --line-length 102 \
  --no-warn-about-unknown-commands
	clang-format -i include/asio/detail/std/*.hpp module/asio-gmf.h CMakePresets.json

#############################################################
# Anything we don't know how to build will use this rule.
#############################################################
% ::
	ninja -C build $(@)
