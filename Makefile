.PHONY: build test run lint format clean module pdf

build:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build

test: build
	ctest --test-dir build --output-on-failure

run: build
ifndef FILE
	$(error Usage: make run FILE=<filename>)
endif
	./build/apps/lab1 examples/$(FILE).signal

LLVM_BIN := $(shell brew --prefix llvm 2>/dev/null)/bin

format:
	$(LLVM_BIN)/clang-format -i apps/*.cpp tests/*.cpp

lint:
	cmake -B build -DCMAKE_CXX_CLANG_TIDY=$(LLVM_BIN)/clang-tidy
	cmake --build build --clean-first

clean:
	rm -rf build

pdf: build
	@test -d .venv || python3 -m venv .venv
	@.venv/bin/pip install -q pygments weasyprint
	@.venv/bin/python scripts/sources-to-pdf.py docs/sources.pdf
	@.venv/bin/python scripts/tests-to-pdf.py docs/tests.pdf

module:
ifndef NAME
	$(error Usage: make module NAME=MyModule)
endif
	@mkdir -p src/$(NAME)
	@printf '#pragma once\n\nclass $(NAME) {\nprivate:\n\npublic:\n    $(NAME)();\n};\n' > src/$(NAME)/$(NAME).hpp
	@printf '#include "$(NAME).hpp"\n\n$(NAME)::$(NAME)() {}\n' > src/$(NAME)/$(NAME).cpp
	@printf '#include <gtest/gtest.h>\n\n#include <$(NAME).hpp>\n\nclass $(NAME)Test : public ::testing::Test {\nprotected:\n};\n' > tests/$(NAME).test.cpp
	@echo "Created src/$(NAME)/$(NAME).hpp"
	@echo "Created src/$(NAME)/$(NAME).cpp"
	@echo "Created tests/$(NAME).test.cpp"
