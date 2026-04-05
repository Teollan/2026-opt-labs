.PHONY: build test run-lab-1 run-lab-2 lint format clean module pdf

build:
	cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
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

SOURCES := $(wildcard apps/*.cpp tests/*.cpp src/*/*.cpp src/*/*.hpp src/*/*.tpp)

format:
	clang-format -i $(SOURCES)

lint:
	cmake -B build -G "MinGW Makefiles" -DCMAKE_CXX_CLANG_TIDY="clang-tidy;--extra-arg=-isystem;--extra-arg=D:/SDKs/msys64/ucrt64/include/c++/14.2.0;--extra-arg=-isystem;--extra-arg=D:/SDKs/msys64/ucrt64/include/c++/14.2.0/x86_64-w64-mingw32"
	cmake --build build --clean-first

clean:
	rm -rf build
