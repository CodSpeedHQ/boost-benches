# Boost Benchmarks

This project contains benchmarks for the Boost C++ library using Google Benchmark.

## Requirements

- CMake (3.10 or higher)
- C++ compiler with C++17 support
- Boost library

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
./boost_bench
```

## Notes

- CodSpeed's fork of Google Benchmark is automatically downloaded and built via CMake's FetchContent
- The benchmarks are parameterized to test with different input sizes (small, medium, large)
- The benchmarks currently focus on:
  - String splitting via boost::algorithm::split (with varying number of items)
  - Integer conversion via boost::lexical_cast (with varying digit counts)
  - Floating-point conversion via boost::lexical_cast (with varying precision)
  - Regular expressions via boost::regex (with varying text sizes)
  - String formatting via boost::format (with varying parameter counts)
  - Type-safe any type via boost::any (with varying operation counts)
  - Container performance comparisons:
    - boost::container::flat_map vs std::map (with varying element counts)
    - boost::container::vector vs std::vector (with varying element counts)
  - Algorithms: boost::algorithm::all_of (with varying collection sizes)
  - UUID generation with boost::uuid (with varying generation counts)
  - Optional types: boost::optional vs std::optional (with varying data sizes)

## Adding new benchmarks

Add new benchmark functions to `src/boost_bench.cpp` following the existing patterns.