# SIGNAL Compiler

C++20 compiler for a subset of the SIGNAL language (variant 6).

Implements:
- **Lab 1** — Lexical analyzer (tokenizer)
- **Lab 2** — Syntax analyzer (recursive-descent parser)

## Quick Start

```bash
make build    # configure + compile
make test     # run all tests
make run FILE=example  # run on examples/<name>.signal
make format   # auto-format source files
make lint     # run clang-tidy (clean rebuild)
make clean    # remove build directory
```

## Prerequisites

- CMake 3.20+
- C++20 compiler (clang, gcc, or MSVC)
- make (macOS/Linux)
- (Optional) clang-format, clang-tidy
- (Optional) Graphviz for syntax tree diagrams

### Windows (without make)

```powershell
cmake -B build
cmake --build build --config Debug
ctest --test-dir build --output-on-failure
```

## Running

After building, binaries are at `build/apps/lab1` and `build/apps/lab2`.

```bash
# Tokenizer
./build/apps/lab1 -s examples/valid-basic.signal -t -i -l

# Parser
./build/apps/lab2 -s examples/valid-basic.signal -t -T -i -l

# Export parse tree as Graphviz DOT
./build/apps/lab2 -s examples/valid-basic.signal -d tree.dot
dot -Tpng -Gdpi=200 tree.dot -o tree.png
```

### lab2 flags

| Flag | Description |
|------|-------------|
| `-s <file>` | Source file (required) |
| `-t` | Print token table |
| `-T` | Print syntax tree (ASCII) |
| `-i` | Print identifiers table |
| `-l` | Print literals table |
| `-d <file>` | Export parse tree as DOT file |

## Report Generation

Requires `pip install pygments weasyprint`.

```bash
python scripts/sources-to-pdf-lab2.py   # source code listing
python scripts/tests-to-pdf-lab2.py     # test results with trees
python scripts/trees-to-jpg.py          # Graphviz tree images
```

Output goes to `docs/lab2/`.
