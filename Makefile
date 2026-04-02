.PHONY: build test run-lab-1 run-lab-2 lint format clean module pdf

build:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build

test: build
	ctest --test-dir build --output-on-failure

run-lab-1: build
ifndef FILE
	$(error Usage: make run-lab-1 FILE=<filename>)
endif
	./build/apps/lab1 examples/$(FILE).signal

run-lab-2: build
ifndef FILE
	$(error Usage: make run-lab-2 FILE=<filename>)
endif
	./build/apps/lab2 examples/$(FILE).signal

SOURCES := $(wildcard apps/*.cpp tests/*.cpp src/*/*.cpp src/*/*.hpp src/*/*.tpp)

format:
	clang-format -i $(SOURCES)

lint:
	cmake -B build -DCMAKE_CXX_CLANG_TIDY=clang-tidy
	cmake --build build --clean-first

clean:
	rm -rf build

pdf: build
	@test -d .venv || python3 -m venv .venv
	@.venv/bin/pip install -q pygments weasyprint
	@.venv/bin/python scripts/sources-to-pdf.py "docs/Козлов КВ-33 ЛАБ1 Додаток 1.pdf"
	@.venv/bin/python scripts/tests-to-pdf.py "docs/Козлов КВ-33 ЛАБ1 Додаток 2.pdf"

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
