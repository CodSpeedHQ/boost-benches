# Boost Benchmarks

A benchmarking suite for comparing performance of various Boost C++ library components using Google Benchmark.

## Requirements

- CMake 3.14+
- C++17 compiler
- Boost 1.87.0+ (with serialization and graph components)

## Building

```bash
# Create build directory
mkdir -p build
cd build

# Configure and build
# For regular timing-based benchmarks:
cmake -DCODSPEED_MODE=walltime ..
# For CodSpeed instrumentation:
cmake -DCODSPEED_MODE=instrumentation ..

make
```

## Running benchmarks

From the build directory:

```bash
# Run individual component benchmarks
./string_bench
./container_bench
./utility_bench
./optional_bench
./spirit_bench
./multiindex_bench
./graph_bench
./serialization_bench

# Run all benchmarks with a single command
cmake --build . --target run_all_benchmarks

# Pass arguments to all benchmarks
cmake --build . --target run_all_benchmarks -- --benchmark_filter=BM_Boost

# Run with regex filter
./string_bench --benchmark_filter="BM_BoostLexicalCast"

# List available benchmarks
./string_bench --benchmark_list_tests
```

## Benchmark Categories

- **string_bench**: String operations, lexical_cast, regex, format
- **container_bench**: Container performance comparisons
- **utility_bench**: Type-safe any, algorithms, UUID generation
- **optional_bench**: Boost vs std::optional
- **spirit_bench**: Parsing operations (CSV, JSON, expressions)
- **multiindex_bench**: Multi-index container operations
- **graph_bench**: Graph algorithms (Dijkstra, A*, BFS, DFS)
- **serialization_bench**: Serialization performance (text, binary, XML)

## Custom Boost Version

Specify a custom Boost version:

```bash
cmake -DBOOST_VERSION=1.82.0 -DCODSPEED_MODE=walltime ..
```

## CodSpeed Integration

This project integrates with [CodSpeed](https://codspeed.io/) for CI performance tracking. For local testing, use:

```bash
cmake -DCODSPEED_MODE=instrumentation ..
make
./string_bench  # Will show notice about running outside CI
```

For CI integration, ensure the `CODSPEED_TOKEN` secret is set in your GitHub repository.