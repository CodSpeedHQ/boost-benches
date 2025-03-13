#include <benchmark/benchmark.h>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <string>
#include <vector>
#include <sstream>

// Benchmark for parsing CSV data with Boost.Spirit
static void BM_SpiritCSVParsing(benchmark::State& state) {
  // Use fixed CSV data
  std::string input = 
    "1,2,3,4,5\n"
    "6,7,8,9,10\n"
    "11,12,13,14,15\n"
    "16,17,18,19,20\n";
  
  for (auto _ : state) {
    namespace qi = boost::spirit::qi;
    
    std::vector<std::vector<int>> result;
    
    auto const row_parser = qi::int_ % ',';
    auto iter = input.begin();
    auto end = input.end();
    
    bool r = qi::phrase_parse(
      iter, end, 
      row_parser % qi::eol, 
      qi::space, 
      result
    );
    
    benchmark::DoNotOptimize(r);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_SpiritCSVParsing);

// Simpler JSON parsing benchmark
static void BM_SpiritJSONParsing(benchmark::State& state) {
  // Fixed simple JSON string
  std::string input = R"(
    {
      "name": "John",
      "age": 30,
      "city": "New York",
      "hobbies": ["reading", "swimming", "cycling"],
      "address": {
        "street": "123 Main St",
        "zip": 10001
      }
    }
  )";
  
  for (auto _ : state) {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    
    // We'll just parse for basic structure validation, not actual data extraction
    qi::rule<std::string::const_iterator, qi::space_type> json_rule;
    qi::rule<std::string::const_iterator, qi::space_type> object, array, pair, value, string;
    
    string = '"' >> *(qi::char_ - '"') >> '"';
    
    value = 
        string
      | qi::long_
      | qi::double_
      | object
      | array
      | qi::string("true")
      | qi::string("false")
      | qi::string("null");
      
    pair = string >> ':' >> value;
    object = '{' >> -(pair % ',') >> '}';
    array = '[' >> -(value % ',') >> ']';
    
    json_rule = object | array;
    
    std::string::const_iterator iter = input.begin();
    std::string::const_iterator end = input.end();
    
    bool r = qi::phrase_parse(iter, end, json_rule, qi::space);
    benchmark::DoNotOptimize(r);
  }
}
BENCHMARK(BM_SpiritJSONParsing);

// Simplified calculator benchmark for Spirit
static void BM_SpiritCalculator(benchmark::State& state) {
  // Use fixed expressions instead of generating them
  std::vector<std::string> expressions = {
    "1 + 2",
    "3 * (4 + 5)",
    "10 - 2 * 3",
    "(1 + 2) * (3 + 4)",
    "5 * 5 * 5 - 25"
  };
  
  for (auto _ : state) {
    namespace qi = boost::spirit::qi;
    
    // A simple calculator grammar
    qi::rule<std::string::const_iterator, int(), qi::space_type> expr, term, factor;
    
    expr = term[qi::_val = qi::_1] >> 
           *('+' >> term[qi::_val += qi::_1] | 
             '-' >> term[qi::_val -= qi::_1]);
             
    term = factor[qi::_val = qi::_1] >> 
           *('*' >> factor[qi::_val *= qi::_1] | 
             '/' >> factor[qi::_val /= qi::_1]);
             
    factor = qi::int_[qi::_val = qi::_1] | 
             '(' >> expr[qi::_val = qi::_1] >> ')';
    
    // Parse all expressions
    int total = 0;
    for (const auto& expression : expressions) {
      int result = 0;
      std::string::const_iterator iter = expression.begin();
      std::string::const_iterator end = expression.end();
      
      bool r = qi::phrase_parse(iter, end, expr, qi::space, result);
      if (r && iter == end) {
        total += result;
      }
    }
    benchmark::DoNotOptimize(total);
  }
}
BENCHMARK(BM_SpiritCalculator);

BENCHMARK_MAIN();