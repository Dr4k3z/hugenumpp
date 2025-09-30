# HugeNum

A tiny C++ library for big-precision decimal arithmetic (grouped base-10 digits).  
Some basic algorithms to compute known mathematical constants like $\pi$, $e$ or $\phi$ are already provided in the `Algorithms` namespace, along with an iterative procedure to compute the square root of integer numbers, based on the bisection method. 
---

## Requirements

- **CMake ≥ 3.31**
- A C++17 compiler
- (Optional, for tests) **GoogleTest** — expected to be cloned **next to** this repo (see below)

---

## Quick start

This project supports the usual Debug / Release plus a custom:

 — MemoryDiagnostic — defines ENABLE_MEMORY_DIAGNOSTICS to activate a custom memory logger (implemented in `MemoryLogger.cpp`) that tracks dynamic memory allocations.

```bash
# Debug
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j

# Release
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# Memory diagnostics
cmake -S . -B build -DCMAKE_BUILD_TYPE=MemoryDiagnostic
cmake --build build -j
./build/hugenumpp            # run with memory tracking enabled
```

# Testing

This CMakeLists expects GoogleTest to live next to this repo:

```
parent/
├─ googletest/        # clone here
└─ hugenumpp/         # this project (your current repo)
```

Some tests need to load files from the `assets` directory. At the moment, the path is passed locally. Hence, if the tests are run from the wrong directory, they will most likely file due to read file error. To ensure assets are correctly read from, run the tests from the `build` directory. A successful run should look like this
```aiignore
[----------] Global test environment tear-down
[==========] 21 tests from 9 test suites ran. (5 ms total)
[  PASSED  ] 21 tests.
```