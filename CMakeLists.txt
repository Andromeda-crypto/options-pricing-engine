cmake_minimum_required(VERSION 3.10)
project(options-pricing-engine LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test_greeks
tests/test_greeks.cpp
src/black_scholes.cpp
src/greeks.cpp
src/random.cpp
)


target_include_directories(test_greeks PRIVATE include)






