#include <benchmark/benchmark.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/any.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/vector.hpp>
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/optional.hpp>
#include <string>
#include <vector>
#include <map>
#include <optional>

// Benchmark for boost::algorithm::split
static void BM_BoostStringSplit(benchmark::State& state) {
  // Create input string based on the range_x parameter (number of items to split)
  int item_count = state.range(0);
  std::string input;
  for (int i = 0; i < item_count; i++) {
    input += "item" + std::to_string(i);
    if (i < item_count - 1) input += ",";
  }
  
  std::vector<std::string> results;
  
  for (auto _ : state) {
    results.clear();
    boost::algorithm::split(results, input, boost::is_any_of(","));
    benchmark::DoNotOptimize(results);
  }
  
  state.counters["ItemCount"] = item_count;
}
BENCHMARK(BM_BoostStringSplit)
  ->Arg(5)    // Small list
  ->Arg(50)   // Medium list
  ->Arg(500); // Large list

// Benchmark for boost::lexical_cast
static void BM_BoostLexicalCast(benchmark::State& state) {
  // Create input string based on the digit count
  int digit_count = state.range(0);
  std::string input;
  
  // Make sure we don't exceed int range
  if (digit_count == 1) {
    input = "9";
  } else if (digit_count == 5) {
    input = "12345";
  } else {
    input = "2000000000";  // Largest safe value for int32
  }
  
  for (auto _ : state) {
    int result = boost::lexical_cast<int>(input);
    benchmark::DoNotOptimize(result);
  }
  
  state.counters["DigitCount"] = digit_count;
}
BENCHMARK(BM_BoostLexicalCast)
  ->Arg(1)    // Single digit
  ->Arg(5)    // Medium number
  ->Arg(10);  // Large number

// Benchmark for string to float conversion
static void BM_BoostLexicalCastFloat(benchmark::State& state) {
  // Create input string based on precision
  int precision = state.range(0);
  std::string input = "123.";
  input += std::string(precision, '9');
  
  for (auto _ : state) {
    float result = boost::lexical_cast<float>(input);
    benchmark::DoNotOptimize(result);
  }
  
  state.counters["Precision"] = precision;
}
BENCHMARK(BM_BoostLexicalCastFloat)
  ->Arg(3)    // Low precision
  ->Arg(6)    // Medium precision
  ->Arg(10);  // High precision

// Benchmark for boost::regex
static void BM_BoostRegex(benchmark::State& state) {
  // Create input text based on parameter
  int text_size = state.range(0);
  
  // Create a text with a fixed pattern repeated multiple times
  std::string base_text = "Hello, my email is john.doe@example.com and my phone is 123-456-7890. ";
  std::string input;
  
  // Repeat the base text to reach approximate desired size
  while (input.length() < text_size) {
    input += base_text;
  }
  
  // Truncate to exact size
  input.resize(text_size);
  
  boost::regex email_pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
  
  for (auto _ : state) {
    boost::smatch match;
    bool result = boost::regex_search(input, match, email_pattern);
    benchmark::DoNotOptimize(result);
    benchmark::DoNotOptimize(match);
  }
  
  state.counters["TextSize"] = text_size;
}
BENCHMARK(BM_BoostRegex)
  ->Arg(100)     // Small text
  ->Arg(1000)    // Medium text
  ->Arg(10000);  // Large text

// Benchmark for boost::format
static void BM_BoostFormat(benchmark::State& state) {
  // Parameter represents the number of substitutions
  int param_count = state.range(0);
  
  // Create format string with varying number of parameters
  std::string fmt_string = "Format with ";
  for (int i = 1; i <= param_count; i++) {
    fmt_string += "param %";
    fmt_string += std::to_string(i);
    fmt_string += "%";
    if (i < param_count) fmt_string += ", ";
  }
  
  boost::format fmt(fmt_string);
  
  for (auto _ : state) {
    // Reset format
    fmt.clear();
    
    // Add all parameters
    boost::format result = fmt;
    for (int i = 0; i < param_count; i++) {
      result = result % ("value" + std::to_string(i));
    }
    
    std::string final_str = result.str();
    benchmark::DoNotOptimize(final_str);
  }
  
  state.counters["ParamCount"] = param_count;
}
BENCHMARK(BM_BoostFormat)
  ->Arg(2)    // Few parameters
  ->Arg(5)    // Medium number of parameters 
  ->Arg(10);  // Many parameters

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

// Benchmark for boost::optional vs std::optional
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