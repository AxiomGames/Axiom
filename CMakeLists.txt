cmake_minimum_required(VERSION 3.22)
project(Axiom)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # set() overrides option()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Axiom src/main.cpp)
