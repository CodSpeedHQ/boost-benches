cmake_minimum_required(VERSION 3.12)
project(boost-bench)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent for downloading Google Benchmark
include(FetchContent)

# Set benchmark download dependencies ON
set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)

# Download and configure CodSpeed C++ Benchmark
FetchContent_Declare(
  google_benchmark
  GIT_REPOSITORY https://github.com/CodSpeedHQ/codspeed-cpp
  SOURCE_SUBDIR google_benchmark
  GIT_TAG v1.0.0
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark testing" FORCE)
FetchContent_MakeAvailable(google_benchmark)

# Find Boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Add benchmark executable
add_executable(boost_bench src/boost_bench.cpp)
target_link_libraries(boost_bench benchmark::benchmark ${Boost_LIBRARIES})
target_include_directories(boost_bench PRIVATE include)