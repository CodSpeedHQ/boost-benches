# Boost Benchmarks

This project contains benchmarks for the Boost C++ library using Google Benchmark. It tests various components of the Boost library with different input sizes and configurations to measure performance characteristics.

## Requirements

- CMake (3.14 or higher)
- C++ compiler with C++17 support
- Boost library with the following components:
  - serialization
  - graph
- Internet connection (for downloading Google Benchmark)

## Building

```bash
# Create build directory
mkdir -p build
cd build

# Configure and build
cmake ..
make
```

## Running benchmarks

From the build directory:

```bash
# Run all benchmarks
./boost_bench

# Run specific benchmarks (using regex filter)
./boost_bench --benchmark_filter="src/string_bench.h::BM_BoostStringSplit"

# List all available benchmarks
./boost_bench --benchmark_list_tests
```

### CodSpeed Integration

This project is set up to run benchmarks in CI using [CodSpeed](https://codspeed.io/). When a pull request is opened or code is pushed to the main branch, GitHub Actions will automatically build and run the benchmarks, reporting the results to CodSpeed.

#### CI Workflow

The GitHub Actions workflow:
1. Installs only the necessary build tools (CMake and GCC)
2. Configures the project with CodSpeed instrumentation (CMake will automatically handle all library dependencies)
3. Builds the project
4. Runs all benchmarks and reports results to CodSpeed

#### Local CodSpeed Testing

To enable CodSpeed instrumentation locally:

```bash
mkdir -p build && cd build
cmake -DCODSPEED_MODE=instrumentation ..
make

# Run the benchmarks with CodSpeed instrumentation
./boost_bench
```

You'll see a message like this when running with CodSpeed instrumentation locally:
```
NOTICE: codspeed is enabled, but no performance measurement will be made since it's running in an unknown environment.
```

This is normal as CodSpeed metrics are only collected in CI environments.

You'll need to set up a `CODSPEED_TOKEN` secret in your GitHub repository settings for the CodSpeed integration to work in CI.

## Notes

- CodSpeed's fork of Google Benchmark is automatically downloaded and configured via CMake's FetchContent
- The benchmarks are parameterized to test with different input sizes (small, medium, large)
- The project is organized into modular benchmark files for better maintainability
- CodSpeed integration allows performance tracking in CI

## Benchmark Categories

### String and Text Processing
- String splitting via boost::algorithm::split
- Integer and floating-point conversion via boost::lexical_cast
- Regular expressions via boost::regex
- String formatting via boost::format

### Containers
- Container performance comparisons:
  - boost::container::flat_map vs std::map
  - boost::container::vector vs std::vector

### Utility Libraries
- Type-safe any type via boost::any
- Algorithms: boost::algorithm::all_of
- UUID generation with boost::uuid
- Optional types: boost::optional vs std::optional

### Multi-Index Containers
- Insertion performance vs standard containers
- Lookup by different indices (ID, email)
- Range-based queries
- Modification with reindexing

### Spirit (Parsing)
- CSV data parsing
- JSON structure parsing
- Calculator expression parsing

### Graph Algorithms
- Dijkstra's shortest path algorithm
- A* search on grid graphs
- Breadth-First Search (BFS)
- Depth-First Search (DFS)

### Serialization
- Text archive performance
- Binary archive performance
- XML archive performance
- Format comparison (text vs binary vs XML)

## Adding new benchmarks

Add new benchmark functions to the corresponding header file in the `src/` directory, or create a new header file for a new category of benchmarks.