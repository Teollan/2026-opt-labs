.PHONY: build test run lint format clean

build:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build

test: build
	ctest --test-dir build --output-on-failure

run: build
	./build/apps/lab1

LLVM_BIN := $(shell brew --prefix llvm 2>/dev/null)/bin

format:
	$(LLVM_BIN)/clang-format -i apps/*.cpp tests/*.cpp

lint:
	cmake -B build -DCMAKE_CXX_CLANG_TIDY=$(LLVM_BIN)/clang-tidy
	cmake --build build --clean-first

clean:
	rm -rf build
