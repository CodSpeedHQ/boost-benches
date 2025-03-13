#ifndef UTILITY_BENCH_H
#define UTILITY_BENCH_H

#include <benchmark/benchmark.h>
#include <boost/any.hpp>
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>
#include <vector>

// Benchmark for boost::any
static void BM_BoostAny(benchmark::State& state) {
  // Parameter represents the number of type changes and casts
  int operations = state.range(0);
  
  for (auto _ : state) {
    boost::any a = 42;
    benchmark::DoNotOptimize(a);
    
    // Perform specified number of operations
    for (int i = 0; i < operations; i++) {
      // Change type based on iteration
      switch (i % 3) {
        case 0:
          a = std::string("Hello" + std::to_string(i));
          break;
        case 1:
          a = 3.14 + i;
          break;
        case 2:
          a = 42 + i;
          break;
      }
      benchmark::DoNotOptimize(a);
      
      // Attempt to cast based on what we know the type is
      if (i % 3 == 0) {
        std::string s = boost::any_cast<std::string>(a);
        benchmark::DoNotOptimize(s);
      }
    }
  }
  
  state.counters["Operations"] = operations;
}
BENCHMARK(BM_BoostAny)
  ->Arg(3)     // Few operations
  ->Arg(10)    // Medium operations
  ->Arg(30);   // Many operations

// Benchmark for boost algorithm
static void BM_BoostAllOf(benchmark::State& state) {
  const int SIZE = state.range(0);
  
  // Create vectors of different sizes
  std::vector<int> v(SIZE);
  for (int i = 0; i < SIZE; ++i) {
    v[i] = i;
  }
  
  for (auto _ : state) {
    bool result = boost::algorithm::all_of(v, [](int x) { return x >= 0; });
    benchmark::DoNotOptimize(result);
  }
  
  state.counters["Elements"] = SIZE;
}
BENCHMARK(BM_BoostAllOf)
  ->Arg(1000)     // Small collection
  ->Arg(10000)    // Medium collection
  ->Arg(100000);  // Large collection

// Benchmark for boost UUID generation
static void BM_BoostUUID(benchmark::State& state) {
  // Number of UUIDs to generate per iteration
  int count = state.range(0);
  boost::uuids::random_generator gen;
  
  for (auto _ : state) {
    for (int i = 0; i < count; i++) {
      boost::uuids::uuid id = gen();
      std::string id_str = boost::uuids::to_string(id);
      benchmark::DoNotOptimize(id);
      benchmark::DoNotOptimize(id_str);
    }
  }
  
  state.counters["UUIDCount"] = count;
}
BENCHMARK(BM_BoostUUID)
  ->Arg(1)      // Single UUID
  ->Arg(5)      // Few UUIDs
  ->Arg(20);    // Many UUIDs

#endif // UTILITY_BENCH_H