cmake_minimum_required(VERSION 3.10)
project(options-pricing-engine LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test_mc
  tests/test_mc.cpp
  src/black_scholes.cpp
  src/monte_carlo.cpp
  src/random.cpp
)

target_include_directories(test_mc PRIVATE include)

add_executable(test_mc_regression
  tests/test_mc_regression.cpp
  src/black_scholes.cpp
  src/monte_carlo.cpp
  src/random.cpp
)
target_include_directories(test_mc_regression PRIVATE include)

add_executable(test_mc_variance_reduction
  tests/test_mc_variance_reduction.cpp
  src/monte_carlo.cpp
  src/random.cpp
)
target_include_directories(test_mc_variance_reduction PRIVATE include)

add_executable(test_mc_edge_cases
  tests/test_mc_edge_cases.cpp
  src/monte_carlo.cpp
  src/random.cpp
)
target_include_directories(test_mc_edge_cases PRIVATE include)

add_executable(performance
  benchmarks/performance.cpp
  src/black_scholes.cpp
  src/monte_carlo.cpp
  src/random.cpp
)
target_include_directories(performance PRIVATE include)

add_executable(options_pricer
  src/main.cpp
  src/black_scholes.cpp
  src/greeks.cpp
  src/monte_carlo.cpp
  src/random.cpp
)

target_include_directories(options_pricer PRIVATE include)










