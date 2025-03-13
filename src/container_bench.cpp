#include <benchmark/benchmark.h>
#include <boost/container/flat_map.hpp>
#include <boost/container/vector.hpp>
#include <vector>
#include <map>

// Benchmark for boost::container::flat_map vs std::map
static void BM_StdMap(benchmark::State& state) {
  const int SIZE = state.range(0);
  
  for (auto _ : state) {
    std::map<int, int> m;
    for (int i = 0; i < SIZE; ++i) {
      m[i] = i * 2;
    }
    
    int sum = 0;
    for (int i = 0; i < SIZE; ++i) {
      sum += m[i];
    }
    benchmark::DoNotOptimize(sum);
  }
  
  state.counters["Elements"] = SIZE;
}
BENCHMARK(BM_StdMap)
  ->Arg(100)     // Small map
  ->Arg(1000)    // Medium map
  ->Arg(10000);  // Large map

static void BM_BoostFlatMap(benchmark::State& state) {
  const int SIZE = state.range(0);
  
  for (auto _ : state) {
    boost::container::flat_map<int, int> m;
    for (int i = 0; i < SIZE; ++i) {
      m[i] = i * 2;
    }
    
    int sum = 0;
    for (int i = 0; i < SIZE; ++i) {
      sum += m[i];
    }
    benchmark::DoNotOptimize(sum);
  }
  
  state.counters["Elements"] = SIZE;
}
BENCHMARK(BM_BoostFlatMap)
  ->Arg(100)     // Small map
  ->Arg(1000)    // Medium map
  ->Arg(10000);  // Large map

// Benchmark for boost::container::vector vs std::vector
static void BM_StdVector(benchmark::State& state) {
  const int SIZE = state.range(0);
  
  for (auto _ : state) {
    std::vector<int> v;
    v.reserve(SIZE);
    for (int i = 0; i < SIZE; ++i) {
      v.push_back(i);
    }
    
    int sum = 0;
    for (int i = 0; i < SIZE; ++i) {
      sum += v[i];
    }
    benchmark::DoNotOptimize(sum);
  }
  
  state.counters["Elements"] = SIZE;
}
BENCHMARK(BM_StdVector)
  ->Arg(1000)     // Small vector
  ->Arg(10000)    // Medium vector
  ->Arg(100000);  // Large vector

static void BM_BoostVector(benchmark::State& state) {
  const int SIZE = state.range(0);
  
  for (auto _ : state) {
    boost::container::vector<int> v;
    v.reserve(SIZE);
    for (int i = 0; i < SIZE; ++i) {
      v.push_back(i);
    }
    
    int sum = 0;
    for (int i = 0; i < SIZE; ++i) {
      sum += v[i];
    }
    benchmark::DoNotOptimize(sum);
  }
  
  state.counters["Elements"] = SIZE;
}
BENCHMARK(BM_BoostVector)
  ->Arg(1000)     // Small vector
  ->Arg(10000)    // Medium vector
  ->Arg(100000);  // Large vector

BENCHMARK_MAIN();