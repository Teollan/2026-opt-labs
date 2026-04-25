.PHONY: build test run-lab-1 run-lab-2 run-lab-3 lint format clean

CXX := g++-14
CC  := gcc-14

build:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=$(CXX) -DCMAKE_C_COMPILER=$(CC)
	cmake --build build

test: build
	ctest --test-dir build --output-on-failure

run-lab-1: build
ifndef FILE
	$(error Usage: make run-lab-1 FILE=<filename>)
endif
	./build/apps/lab1 --source examples/$(FILE).signal -t -i -l

run-lab-2: build
ifndef FILE
	$(error Usage: make run-lab-2 FILE=<filename>)
endif
	./build/apps/lab2 --source examples/$(FILE).signal -T

run-lab-3: build
ifndef FILE
	$(error Usage: make run-lab-3 FILE=<filename>)
endif
	./build/apps/lab3 --source examples/$(FILE).signal -d -o asm/$(FILE).asm

SOURCES := $(wildcard apps/*.cpp tests/*.cpp src/*/*.cpp src/*/*.hpp src/*/*.tpp)

format:
	clang-format -i $(SOURCES)

lint:
	cmake -B build -DCMAKE_CXX_CLANG_TIDY="clang-tidy" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=$(CXX) -DCMAKE_C_COMPILER=$(CC)
	cmake --build build --clean-first

clean:
	rm -rf build