# SIGNAL Compiler

C++ project for SIGNAL language compiler.

## Quick Start

```bash
make build    # configure + compile
make test     # run all tests
make run      # run the app
make format   # auto-format source files
make lint     # run clang-tidy (clean rebuild)
make clean    # remove build directory
```

## Prerequisites

- CMake 3.20+
- C++17 compiler (clang, gcc, or MSVC)
- make (macOS/Linux)
- (Optional) clang-format, clang-tidy

### Windows (without make)

```powershell
cmake -B build
cmake --build build --config Debug
ctest --test-dir build --output-on-failure
```
