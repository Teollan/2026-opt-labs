# SIGNAL Compiler

C++20 compiler for a subset of the SIGNAL language (variant 6).

Implements:
- **Lab 1** — Lexical analyzer (tokenizer)
- **Lab 2** — Syntax analyzer (recursive-descent parser)
- **Lab 3** — Code generator (semantic analysis + x86-64 assembly emission)

## Quick Start

```bash
make build              # configure + compile (Debug)
make test               # build + run all tests
make run-lab-1 FILE=valid-basic   # tokenizer
make run-lab-2 FILE=valid-basic   # parser
make run-lab-3 FILE=valid-basic   # codegen → asm/<file>.asm
make format             # clang-format all sources
make lint               # clean rebuild with clang-tidy
make clean              # remove build directory
```

## Prerequisites

- CMake 3.20+
- C++20 compiler (clang, gcc, or MSVC)
- make (macOS/Linux)
- (Optional) clang-format, clang-tidy
- (Optional) Graphviz for syntax tree diagrams
- (Optional) Python 3 + `pygments` + `weasyprint` for report generation

### Windows (without make)

```powershell
cmake -B build
cmake --build build --config Debug
ctest --test-dir build --output-on-failure
```

## Running

After building, binaries are at `build/apps/lab1`, `build/apps/lab2`, and `build/apps/lab3`.

```bash
# Tokenizer
./build/apps/lab1 -s examples/valid-basic.signal -t -i -l

# Parser
./build/apps/lab2 -s examples/valid-basic.signal -t -T -i -l

# Code generator (full pipeline + assembly)
./build/apps/lab3 -s examples/valid-basic.signal -d -a -o out.asm

# Export parse tree as Graphviz DOT
./build/apps/lab2 -s examples/valid-basic.signal -D tree.dot
dot -Tpng -Gdpi=200 tree.dot -o tree.png
```

### Flags

| Flag | lab1 | lab2 | lab3 | Description |
|------|:---:|:---:|:---:|-------------|
| `-s <file>` | ✓ | ✓ | ✓ | Source file (required) |
| `-t` | ✓ | ✓ | ✓ | Print token table |
| `-T` |   | ✓ | ✓ | Print syntax tree (ASCII) |
| `-i` | ✓ | ✓ | ✓ | Print identifiers table |
| `-l` | ✓ | ✓ | ✓ | Print literals table |
| `-D <file>` |   | ✓ |   | Export parse tree as DOT file |
| `-d` |   |   | ✓ | Print declarations table |
| `-a` |   |   | ✓ | Print generated assembly |
| `-o <file>` |   |   | ✓ | Write assembly to file |

## Project Structure

```
apps/        # CLI entry points (lab1.cpp, lab2.cpp, lab3.cpp)
src/         # Library modules — auto-discovered by CMake into compiler_lib
  tokenizer/
  parser/
  grammar/
  ast/         # Parse tree → AST folding
  semantics/   # Semantic analyzer (duplicate-identifier checks)
  declarations/# Declarations table (codegen-ready)
  types/       # Type / TypeModifier enums
  codegen/     # Assembly emission
  symbols/     # Symbol store (keywords, delimiters, identifiers, literals)
  ui/          # Table/tree/log views
  utility/     # Shared containers (Tree, Stack, Queue, etc.)
tests/       # GoogleTest suites, auto-discovered as *.test.cpp
examples/    # SIGNAL test programs (valid-*, lexical-error-*, syntax-error-*, semantic-error-*)
docs/        # Assignment.pdf, Grammar.md, lab reports + appendices
scripts/     # Report-generation helpers (Python)
```

The grammar lives in `docs/Grammar.md`. All library code is auto-discovered by `src/CMakeLists.txt`; new modules just need to follow the `src/<module>/<Module>.{hpp,cpp}` convention.

## Report Generation

Requires Python 3 with `pygments` and `weasyprint`. Set up a venv:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install pygments weasyprint
```

On Linux, WeasyPrint may need system libs (`libpango-1.0-0`, `libpangoft2-1.0-0`).

```bash
# Lab 2 (parser) appendices
python scripts/sources-to-pdf-lab2.py   # source code listing
python scripts/tests-to-pdf-lab2.py     # test results

# Lab 3 (codegen) appendices
python scripts/sources-to-pdf-lab3.py   # source code listing
python scripts/tests-to-pdf-lab3.py     # test results with assembly + tables

# Graphviz tree images
python scripts/trees-to-jpg.py
```

Each script accepts an optional output path as its first argument; defaults are written to `docs/lab2/` or `docs/lab3/`.
