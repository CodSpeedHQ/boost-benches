#include <benchmark/benchmark.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <string>
#include <vector>

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

BENCHMARK_MAIN();