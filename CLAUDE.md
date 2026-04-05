# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

University lab project: a compiler for the SIGNAL language (variant 6), written in C++20. Implements a tokenizer/lexer (lab1) and a recursive-descent parser (lab2). The grammar is defined in `docs/Grammar.md`.

## Build & Test Commands

```bash
make build                  # CMake configure + compile (Debug)
make test                   # build + run all tests via ctest
make run FILE=constants     # build + run on examples/<name>.signal
make format                 # clang-format all sources
make lint                   # rebuild with clang-tidy enabled
make module NAME=Foo        # scaffold new module (hpp/cpp/test)
```

On Windows without make:
```powershell
cmake -B build
cmake --build build --config Debug
ctest --test-dir build --output-on-failure
```

Binary location: `build/apps/lab1` and `build/apps/lab2` (Unix) or `build/apps/lab1.exe` and `build/apps/lab2.exe` (Windows).

## Architecture

All library code lives in `src/`, each module in its own subdirectory (`src/ModuleName/ModuleName.hpp`). The `src/CMakeLists.txt` auto-discovers all `**/*.cpp` and exposes them as `compiler_lib`. Tests in `tests/` are auto-discovered `*.test.cpp` files linked against `compiler_lib` and GoogleTest.

Key modules:
- **Source** (abstract) / **FileSource** / **StringSource** — character-level input with row/column tracking via `SourcePosition`
- **CharacterAttributes** — classifies characters into categories for the tokenizer
- **Tokenizer** — lexical scanner; reads from a `Source`, produces tokens, stores symbols in `SymbolStore`
- **SymbolStore** — symbol table holding keywords, delimiters, identifiers, and literals with numeric codes
- **Constants** — predefined keyword/delimiter tables
- **Parser** — recursive-descent syntax analyzer; builds a `Tree<SyntaxData>` parse tree from tokens; includes error recovery (panic mode + automatic semicolon insertion)
- **SyntaxData** / **SyntaxError** — parse tree node data and syntax error representation
- **Queue** / **Stack** — generic FIFO/LIFO containers used by the parser
- **Tree** / **TreeNode** — generic tree structure for the parse tree
- **SyntaxTreeView** — ASCII box-drawing tree renderer with rule numbers
- **DotTreeView** — Graphviz DOT file exporter for parse trees
- **Table** / **Log** — output formatting utilities

## Code Style

- Google-based clang-format with 80-column limit, 4-space indent
- clang-tidy checks: `bugprone-*`, `modernize-*`, `readability-*` (with select exclusions; see `.clang-tidy` files in root, `src/`, `tests/`)
- Header-only templates use `.tpp` extension (included at bottom of `.hpp`)
- Headers use `#pragma once`
- Private member variables use `_` prefix (e.g. `_data`, `_root`)
- Prefer one field per line when constructing structs inline with designated initializers:
  ```cpp
  auto scope = grow({
      .symbol = "<program>",
      .rule = "1",
  });
  ```
