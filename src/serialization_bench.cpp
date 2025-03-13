#include <benchmark/benchmark.h>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <unordered_map>
#include <random>

// Define a complex data structure for serialization testing
class ComplexData {
private:
    int id;
    std::string name;
    std::vector<double> values;
    std::map<std::string, int> properties;
    std::set<std::string> tags;
    std::string extra;
    std::unordered_map<int, std::vector<std::string>> nestedData;

    friend class boost::serialization::access;
    
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const {
        ar & BOOST_SERIALIZATION_NVP(id);
        ar & BOOST_SERIALIZATION_NVP(name);
        ar & BOOST_SERIALIZATION_NVP(values);
        ar & BOOST_SERIALIZATION_NVP(properties);
        ar & BOOST_SERIALIZATION_NVP(tags);
        ar & BOOST_SERIALIZATION_NVP(extra);
        
        if (version > 0) {
            ar & BOOST_SERIALIZATION_NVP(nestedData);
        }
    }
    
    template<class Archive>
    void load(Archive & ar, const unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(id);
        ar & BOOST_SERIALIZATION_NVP(name);
        ar & BOOST_SERIALIZATION_NVP(values);
        ar & BOOST_SERIALIZATION_NVP(properties);
        ar & BOOST_SERIALIZATION_NVP(tags);
        ar & BOOST_SERIALIZATION_NVP(extra);
        
        if (version > 0) {
            ar & BOOST_SERIALIZATION_NVP(nestedData);
        }
    }
    
    BOOST_SERIALIZATION_SPLIT_MEMBER()

public:
    ComplexData() {}
    
    ComplexData(int id, const std::string& name) 
        : id(id), name(name), extra("extra data") {}
    
    void addValue(double value) {
        values.push_back(value);
    }
    
    void addProperty(const std::string& key, int value) {
        properties[key] = value;
    }
    
    void addTag(const std::string& tag) {
        tags.insert(tag);
    }
    
    void addNestedData(int key, const std::vector<std::string>& data) {
        nestedData[key] = data;
    }
};

BOOST_CLASS_VERSION(ComplexData, 1)

// Generate complex test data with parameterized size
ComplexData generateComplexData(int size) {
    ComplexData data(1234, "TestObject");
    
    // Add values
    for (int i = 0; i < size; ++i) {
        data.addValue(i * 3.14159);
    }
    
    // Add properties
    for (int i = 0; i < size / 2; ++i) {
        data.addProperty("prop_" + std::to_string(i), i * 10);
    }
    
    // Add tags
    for (int i = 0; i < size / 3; ++i) {
        data.addTag("tag_" + std::to_string(i));
    }
    
    // Add nested data
    for (int i = 0; i < size / 5; ++i) {
        std::vector<std::string> strings;
        for (int j = 0; j < 3; ++j) {
            strings.push_back("nested_" + std::to_string(i) + "_" + std::to_string(j));
        }
        data.addNestedData(i, strings);
    }
    
    return data;
}

// Generate a vector of complex data for bulk serialization
std::vector<ComplexData> generateDataVector(int count, int itemSize) {
    std::vector<ComplexData> dataVector;
    
    for (int i = 0; i < count; ++i) {
        dataVector.push_back(generateComplexData(itemSize));
    }
    
    return dataVector;
}

// Benchmark for text archives
static void BM_BoostSerializationText(benchmark::State& state) {
    int item_count = state.range(0);
    int item_size = state.range(1);
    
    // Generate test data
    auto testData = generateDataVector(item_count, item_size);
    
    for (auto _ : state) {
        std::ostringstream oss;
        
        // Serialize
        {
            boost::archive::text_oarchive oa(oss);
            oa << testData;
        }
        
        std::string serialized = oss.str();
        benchmark::DoNotOptimize(serialized);
        
        // Deserialize
        std::vector<ComplexData> loadedData;
        std::istringstream iss(serialized);
        {
            boost::archive::text_iarchive ia(iss);
            ia >> loadedData;
        }
        
        benchmark::DoNotOptimize(loadedData);
    }
    
    state.counters["ItemCount"] = item_count;
    state.counters["ItemSize"] = item_size;
}
BENCHMARK(BM_BoostSerializationText)
    ->Args({1, 10})     // Single small item
    ->Args({10, 100})   // Few medium items
    ->Args({100, 50});  // Many small-medium items

// Benchmark for binary archives
static void BM_BoostSerializationBinary(benchmark::State& state) {
    int item_count = state.range(0);
    int item_size = state.range(1);
    
    // Generate test data
    auto testData = generateDataVector(item_count, item_size);
    
    for (auto _ : state) {
        std::ostringstream oss;
        
        // Serialize
        {
            boost::archive::binary_oarchive oa(oss);
            oa << testData;
        }
        
        std::string serialized = oss.str();
        benchmark::DoNotOptimize(serialized);
        
        // Deserialize
        std::vector<ComplexData> loadedData;
        std::istringstream iss(serialized);
        {
            boost::archive::binary_iarchive ia(iss);
            ia >> loadedData;
        }
        
        benchmark::DoNotOptimize(loadedData);
    }
    
    state.counters["ItemCount"] = item_count;
    state.counters["ItemSize"] = item_size;
}
BENCHMARK(BM_BoostSerializationBinary)
    ->Args({1, 10})     // Single small item
    ->Args({10, 100})   // Few medium items
    ->Args({100, 50});  // Many small-medium items

// Benchmark for XML archives
static void BM_BoostSerializationXML(benchmark::State& state) {
    int item_count = state.range(0);
    int item_size = state.range(1);
    
    // Generate test data
    auto testData = generateDataVector(item_count, item_size);
    
    for (auto _ : state) {
        std::ostringstream oss;
        
        // Serialize
        {
            boost::archive::xml_oarchive oa(oss);
            oa << BOOST_SERIALIZATION_NVP(testData);
        }
        
        std::string serialized = oss.str();
        benchmark::DoNotOptimize(serialized);
        
        // Deserialize
        std::vector<ComplexData> loadedData;
        std::istringstream iss(serialized);
        {
            boost::archive::xml_iarchive ia(iss);
            ia >> BOOST_SERIALIZATION_NVP(loadedData);
        }
        
        benchmark::DoNotOptimize(loadedData);
    }
    
    state.counters["ItemCount"] = item_count;
    state.counters["ItemSize"] = item_size;
}
BENCHMARK(BM_BoostSerializationXML)
    ->Args({1, 10})    // Single small item
    ->Args({10, 20})   // Few small items
    ->Args({20, 10});  // More small items (XML is slower)

// Benchmark comparing serialization only (without deserialization)
static void BM_BoostSerializationCompareFormats(benchmark::State& state) {
    int format = state.range(0); // 0 = text, 1 = binary, 2 = XML
    int size = state.range(1);
    
    // Generate a single complex object
    auto testData = generateComplexData(size);
    
    for (auto _ : state) {
        std::ostringstream oss;
        
        // Serialize based on format
        switch (format) {
            case 0: {
                boost::archive::text_oarchive oa(oss);
                oa << testData;
                break;
            }
            case 1: {
                boost::archive::binary_oarchive oa(oss);
                oa << testData;
                break;
            }
            case 2: {
                boost::archive::xml_oarchive oa(oss);
                oa << BOOST_SERIALIZATION_NVP(testData);
                break;
            }
        }
        
        std::string serialized = oss.str();
        benchmark::DoNotOptimize(serialized);
    }
    
    std::string format_name;
    switch (format) {
        case 0: format_name = "Text"; break;
        case 1: format_name = "Binary"; break;
        case 2: format_name = "XML"; break;
    }
    
    state.counters["Format"] = format;
    state.counters["FormatName"] = static_cast<double>(format);
    state.counters["Size"] = size;
}
BENCHMARK(BM_BoostSerializationCompareFormats)
    ->Args({0, 1000})   // Text, large
    ->Args({1, 1000})   // Binary, large
    ->Args({2, 1000});  // XML, large

BENCHMARK_MAIN();