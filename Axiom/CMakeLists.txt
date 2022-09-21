#axiom/axiom
project(Axiom)

file(GLOB_RECURSE AXIOM_SOURCES src/*.hpp src/*.cpp)
add_library(Axiom ${AXIOM_SOURCES})
target_include_directories(Axiom PUBLIC src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")

target_link_libraries(Axiom PUBLIC glfw EnTT d3d11 d3d12 dxgi dxguid d3dcompiler)
target_compile_definitions(Axiom PUBLIC NOMINMAX)