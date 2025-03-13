#include <benchmark/benchmark.h>
#include <boost/optional.hpp>
#include <string>
#include <optional>

// Test data structure for optional benchmarks
struct TestData {
    int a;
    double b;
    std::string c;
};

// Create test data of varying sizes
TestData createTestData(int size) {
    return TestData{
        42,
        3.14159,
        std::string(size, 'x')  // String of length 'size'
    };
}

static void BM_StdOptional(benchmark::State& state) {
  // Parameter represents the size of the string in TestData
  int data_size = state.range(0);
  TestData data = createTestData(data_size);
  
  for (auto _ : state) {
    std::optional<TestData> opt;
    benchmark::DoNotOptimize(opt);
    
    // Set value
    opt = data;
    benchmark::DoNotOptimize(opt);
    
    // Access value
    if (opt) {
      benchmark::DoNotOptimize(opt->a);
      benchmark::DoNotOptimize(opt->b);
      benchmark::DoNotOptimize(opt->c);
    }
    
    // Reset value
    opt.reset();
    benchmark::DoNotOptimize(opt);
  }
  
  state.counters["DataSize"] = data_size;
}
BENCHMARK(BM_StdOptional)
  ->Arg(10)      // Small data
  ->Arg(100)     // Medium data
  ->Arg(1000);   // Large data

static void BM_BoostOptional(benchmark::State& state) {
  // Parameter represents the size of the string in TestData
  int data_size = state.range(0);
  TestData data = createTestData(data_size);
  
  for (auto _ : state) {
    boost::optional<TestData> opt;
    benchmark::DoNotOptimize(opt);
    
    // Set value
    opt = data;
    benchmark::DoNotOptimize(opt);
    
    // Access value
    if (opt) {
      benchmark::DoNotOptimize(opt->a);
      benchmark::DoNotOptimize(opt->b);
      benchmark::DoNotOptimize(opt->c);
    }
    
    // Reset value
    opt.reset();
    benchmark::DoNotOptimize(opt);
  }
  
  state.counters["DataSize"] = data_size;
}
BENCHMARK(BM_BoostOptional)
  ->Arg(10)      // Small data
  ->Arg(100)     // Medium data
  ->Arg(1000);   // Large data

BENCHMARK_MAIN();