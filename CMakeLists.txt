cmake_minimum_required(VERSION 3.22)
project(Axiom)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # set() overrides option()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
	add_link_options(/DEBUG:FULL)
	add_compile_options(/Oi /GR- /D_HAS_EXCEPTIONS=0)
endif()

add_subdirectory(lib)

file(GLOB_RECURSE AXIOM_SOURCES src/*.hpp src/*.cpp)
add_executable(Axiom ${AXIOM_SOURCES})
target_include_directories(Axiom PUBLIC src)

target_link_libraries(Axiom PUBLIC glfw EnTT d3d11 dxgi dxguid d3dcompiler)

target_compile_definitions(Axiom PUBLIC NOMINMAX)