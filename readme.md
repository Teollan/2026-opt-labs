# SIGNAL Compiler

C++ project for SIGNAL language compiler.

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

### Windows (without make)

```powershell
cmake -B build
cmake --build build --config Debug
ctest --test-dir build --output-on-failure
```

## Running the Binary

After building, the binary is located at:

- macOS/Linux: `build/apps/lab1`
- Windows: `build/apps/Debug/lab1.exe`

```bash
./build/apps/lab1 <path-to-file.signal>
```
