cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(boost-bench)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option for CodSpeed instrumentation mode
option(CODSPEED_MODE "Build for CodSpeed instrumentation" OFF)

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

# Configure benchmark options
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark testing" FORCE)
if(CODSPEED_MODE)
  set(BENCHMARK_CODSPEED_MODE "instrumentation" CACHE STRING "CodSpeed mode" FORCE)
  message(STATUS "Building with CodSpeed instrumentation enabled")
endif()

# Disable -Werror for CodSpeed Benchmark to avoid the unused private field error
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wno-error=unused-private-field)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  add_compile_options(-Wno-error)
endif()

FetchContent_MakeAvailable(google_benchmark)

# Add Boost
find_package(Boost REQUIRED 
  COMPONENTS 
    serialization 
    graph
)
include_directories(${Boost_INCLUDE_DIRS})

# Add benchmark executable
add_executable(boost_bench src/boost_bench.cpp)
target_link_libraries(boost_bench 
  benchmark::benchmark 
  ${Boost_LIBRARIES}
)
target_include_directories(boost_bench PRIVATE include)

# Print configuration info
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "CodSpeed mode: ${CODSPEED_MODE}")