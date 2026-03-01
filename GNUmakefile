# Standard stuff

.SUFFIXES:

MAKEFLAGS+= --no-builtin-rules  # Disable the built-in implicit rules.
# MAKEFLAGS+= --warn-undefined-variables  # Warn when an undefined variable is referenced.

.PHONY: all test install check clean

all: build
	ninja -C build all all_verify_interface_header_sets

build: GNUmakefile CMakeLists.txt
	cmake --version
	CXX=clang++ cmake -G Ninja -S . -B build -D CMAKE_CXX_STANDARD=23 -D CMAKE_BUILD_TYPE=Release --fresh -Wdev
	ln -fs build/compile_commands.json .

clean:
	rm -rf build .cache compile_commands.json
	find . -name .DS_Store -delete
	find . -name '*~' -delete

check: build
	run-clang-tidy src/examples/cpp20

install test: build
	ninja -C build $(@)

# Anything we don't know how to build will use this rule.
% ::
	ninja -C build $(@)
