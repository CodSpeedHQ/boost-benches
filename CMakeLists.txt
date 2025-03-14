cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(boost-bench)

# TODO: remove this once the CodSpeed Benchmark is fixed
# Disable -Werror for CodSpeed Benchmark to avoid the unused private field error
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wno-error=unused-private-field)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  add_compile_options(-Wno-error)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT DEFINED BOOST_VERSION)
  set(BOOST_VERSION "1.85.0")
endif()


include(FetchContent)
include(cmake/CPM.cmake)

set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)
FetchContent_Declare(
  google_benchmark
  GIT_REPOSITORY https://github.com/CodSpeedHQ/codspeed-cpp
  SOURCE_SUBDIR google_benchmark
  GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(google_benchmark)

CPMAddPackage(
  NAME Boost
  VERSION ${BOOST_VERSION} # Versions less than 1.85.0 may need patches for installation targets.
  URL https://github.com/boostorg/boost/releases/download/boost-${BOOST_VERSION}/boost-${BOOST_VERSION}-cmake.tar.xz
  OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_SKIP_INSTALL_RULES ON" # Set `OFF` for installation
          "BUILD_SHARED_LIBS OFF" "BOOST_INCLUDE_LIBRARIES container\\\;asio\\\;format\\\;any\\\;uuid\\\;spirit\\\;serialization\\\;graph"
          "CMAKE_BUILD_TYPE RelWithDebInfo"
)
set(BOOST_LIBRARIES Boost::container Boost::asio Boost::format Boost::any Boost::uuid Boost::spirit Boost::serialization Boost::graph)


# Create individual benchmark executables
set(BENCHMARKS string_bench container_bench utility_bench optional_bench spirit_bench multiindex_bench graph_bench serialization_bench)

foreach(benchmark IN LISTS BENCHMARKS)
  add_executable(${benchmark} src/${benchmark}.cpp)
  target_link_libraries(${benchmark}
    benchmark::benchmark
    ${BOOST_LIBRARIES}
  )
endforeach()

add_custom_target(run_all_benchmarks
  COMMAND ${CMAKE_COMMAND} -E echo "Running all benchmarks..."
)

# Make each benchmark depend on the run_all_benchmarks target
foreach(benchmark IN LISTS BENCHMARKS)
  add_custom_command(
    TARGET run_all_benchmarks
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Running ${benchmark}..."
    COMMAND $<TARGET_FILE:${benchmark}>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endforeach()

# Print configuration info
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Boost version: ${BOOST_VERSION}")
message(STATUS "CodSpeed mode: ${CODSPEED_MODE}")
