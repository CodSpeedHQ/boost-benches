#ifndef MULTIINDEX_BENCH_H
#define MULTIINDEX_BENCH_H

#include <benchmark/benchmark.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>

namespace bmi = boost::multi_index;

// Structure representing a person with multiple fields for indexing
struct Person {
    int id;
    std::string name;
    std::string email;
    int age;
    std::string city;
    
    Person(int i, const std::string& n, const std::string& e, int a, const std::string& c)
      : id(i), name(n), email(e), age(a), city(c) {}
};

// Tags for the different indices
struct id {};
struct name {};
struct email {};
struct age {};
struct city {};

// Boost.MultiIndex container with multiple indices
typedef boost::multi_index_container<
  Person,
  bmi::indexed_by<
    bmi::ordered_unique<bmi::tag<id>, bmi::member<Person, int, &Person::id>>,
    bmi::hashed_unique<bmi::tag<email>, bmi::member<Person, std::string, &Person::email>>,
    bmi::ordered_non_unique<bmi::tag<name>, bmi::member<Person, std::string, &Person::name>>,
    bmi::ordered_non_unique<bmi::tag<age>, bmi::member<Person, int, &Person::age>>,
    bmi::ordered_non_unique<bmi::tag<city>, bmi::member<Person, std::string, &Person::city>>,
    bmi::random_access<>
  >
> person_multi_index;

// Generate test data
std::vector<Person> generatePersons(int count) {
  std::vector<Person> persons;
  persons.reserve(count);
  
  std::vector<std::string> names = {"John", "Mary", "Steve", "Jane", "Michael", "Sarah", "Robert", "Emily", "William", "Olivia"};
  std::vector<std::string> cities = {"New York", "London", "Paris", "Tokyo", "Berlin", "Sydney", "Moscow", "Beijing", "Mumbai", "Rio"};
  
  for (int i = 0; i < count; ++i) {
    int id = i;
    std::string name = names[i % names.size()];
    std::string email = name + std::to_string(i) + "@example.com";
    int age = 20 + (i % 60); // ages 20-79
    std::string city = cities[i % cities.size()];
    
    persons.emplace_back(id, name, email, age, city);
  }
  
  return persons;
}

// Benchmark for inserting into a multi_index_container
static void BM_MultiIndexInsert(benchmark::State& state) {
  const int SIZE = state.range(0);
  auto persons = generatePersons(SIZE);
  
  for (auto _ : state) {
    person_multi_index container;
    
    // Insert all persons
    for (const auto& person : persons) {
      container.insert(person);
    }
    
    benchmark::DoNotOptimize(container);
  }
  
  state.counters["Elements"] = SIZE;
}
BENCHMARK(BM_MultiIndexInsert)
  ->Arg(100)     // Small dataset
  ->Arg(1000)    // Medium dataset
  ->Arg(10000);  // Large dataset

// Comparison benchmark using standard containers
static void BM_StandardContainersInsert(benchmark::State& state) {
  const int SIZE = state.range(0);
  auto persons = generatePersons(SIZE);
  
  for (auto _ : state) {
    // Create separate data structures for each index
    std::map<int, size_t> id_index;
    std::unordered_map<std::string, size_t> email_index;
    std::multimap<std::string, size_t> name_index;
    std::multimap<int, size_t> age_index;
    std::multimap<std::string, size_t> city_index;
    std::multimap<std::pair<std::string, std::string>, size_t> name_city_index;
    std::vector<Person> data;
    
    // Insert all persons
    for (size_t i = 0; i < persons.size(); ++i) {
      const auto& person = persons[i];
      
      // Store in vector
      data.push_back(person);
      
      // Update indices
      id_index[person.id] = i;
      email_index[person.email] = i;
      name_index.emplace(person.name, i);
      age_index.emplace(person.age, i);
      city_index.emplace(person.city, i);
      name_city_index.emplace(std::make_pair(person.name, person.city), i);
    }
    
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(id_index);
    benchmark::DoNotOptimize(email_index);
    benchmark::DoNotOptimize(name_index);
    benchmark::DoNotOptimize(age_index);
    benchmark::DoNotOptimize(city_index);
    benchmark::DoNotOptimize(name_city_index);
  }
  
  state.counters["Elements"] = SIZE;
}
BENCHMARK(BM_StandardContainersInsert)
  ->Arg(100)     // Small dataset
  ->Arg(1000)    // Medium dataset
  ->Arg(10000);  // Large dataset

// Benchmark for lookup by different indices
static void BM_MultiIndexLookupById(benchmark::State& state) {
  const int SIZE = state.range(0);
  auto persons = generatePersons(SIZE);
  
  // Prepare container
  person_multi_index container;
  for (const auto& person : persons) {
    container.insert(person);
  }
  
  // Prepare lookup keys
  std::vector<int> lookup_ids;
  for (int i = 0; i < 100; ++i) {
    lookup_ids.push_back(rand() % SIZE);
  }
  
  for (auto _ : state) {
    int sum = 0;
    auto& id_index = container.get<id>();
    
    for (auto id : lookup_ids) {
      auto it = id_index.find(id);
      if (it != id_index.end()) {
        sum += it->age;
      }
    }
    
    benchmark::DoNotOptimize(sum);
  }
  
  state.counters["Elements"] = SIZE;
  state.counters["Lookups"] = lookup_ids.size();
}
BENCHMARK(BM_MultiIndexLookupById)
  ->Arg(1000)    // Medium dataset
  ->Arg(10000);  // Large dataset

static void BM_MultiIndexLookupByEmail(benchmark::State& state) {
  const int SIZE = state.range(0);
  auto persons = generatePersons(SIZE);
  
  // Prepare container
  person_multi_index container;
  for (const auto& person : persons) {
    container.insert(person);
  }
  
  // Prepare lookup keys
  std::vector<std::string> lookup_emails;
  for (int i = 0; i < 100; ++i) {
    int idx = rand() % SIZE;
    lookup_emails.push_back(persons[idx].email);
  }
  
  for (auto _ : state) {
    int sum = 0;
    auto& email_index = container.get<email>();
    
    for (const auto& email : lookup_emails) {
      auto it = email_index.find(email);
      if (it != email_index.end()) {
        sum += it->age;
      }
    }
    
    benchmark::DoNotOptimize(sum);
  }
  
  state.counters["Elements"] = SIZE;
  state.counters["Lookups"] = lookup_emails.size();
}
BENCHMARK(BM_MultiIndexLookupByEmail)
  ->Arg(1000)    // Medium dataset
  ->Arg(10000);  // Large dataset

static void BM_MultiIndexRangeByAge(benchmark::State& state) {
  const int SIZE = state.range(0);
  auto persons = generatePersons(SIZE);
  
  // Prepare container
  person_multi_index container;
  for (const auto& person : persons) {
    container.insert(person);
  }
  
  // Age ranges to look up
  struct AgeRange { int min; int max; };
  std::vector<AgeRange> age_ranges = {
    {20, 30}, {30, 40}, {40, 50}, {50, 60}, {60, 70}
  };
  
  for (auto _ : state) {
    int total_count = 0;
    auto& age_index = container.get<age>();
    
    for (const auto& range : age_ranges) {
      auto lower = age_index.lower_bound(range.min);
      auto upper = age_index.upper_bound(range.max);
      
      int count = 0;
      for (auto it = lower; it != upper; ++it) {
        count++;
      }
      
      total_count += count;
    }
    
    benchmark::DoNotOptimize(total_count);
  }
  
  state.counters["Elements"] = SIZE;
  state.counters["RangeCount"] = age_ranges.size();
}
BENCHMARK(BM_MultiIndexRangeByAge)
  ->Arg(1000)    // Medium dataset
  ->Arg(10000);  // Large dataset


// Benchmark for modification operations with reindexing
static void BM_MultiIndexModify(benchmark::State& state) {
  const int SIZE = state.range(0);
  auto persons = generatePersons(SIZE);
  
  // Number of modifications to perform
  int mod_count = state.range(1);
  
  for (auto _ : state) {
    // Prepare container
    person_multi_index container;
    for (const auto& person : persons) {
      container.insert(person);
    }
    
    auto& id_index = container.get<id>();
    
    // Perform modifications (changing name and city which affects multiple indices)
    for (int i = 0; i < mod_count; ++i) {
      int id_to_modify = rand() % SIZE;
      auto it = id_index.find(id_to_modify);
      
      if (it != id_index.end()) {
        // Modify the record - this will update all indices
        id_index.modify(it, [](Person& p) {
          p.name = "Modified" + std::to_string(rand() % 1000);
          p.city = "NewCity" + std::to_string(rand() % 1000);
        });
      }
    }
    
    benchmark::DoNotOptimize(container);
  }
  
  state.counters["Elements"] = SIZE;
  state.counters["Modifications"] = mod_count;
}
BENCHMARK(BM_MultiIndexModify)
  ->Args({1000, 10})    // Medium dataset, few modifications
  ->Args({1000, 100})   // Medium dataset, many modifications
  ->Args({10000, 100}); // Large dataset, many modifications

#endif // MULTIINDEX_BENCH_H