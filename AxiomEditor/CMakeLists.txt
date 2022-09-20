project(AxiomEditor)

file(GLOB_RECURSE AXIOM_SOURCES src/*.hpp src/*.cpp)
add_executable(AxiomEditor ${AXIOM_SOURCES})
target_include_directories(AxiomEditor PUBLIC src)

target_link_libraries(AxiomEditor PUBLIC Axiom)