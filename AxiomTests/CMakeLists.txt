

file(GLOB_RECURSE AXIOM_TEST_SOURCES src/*.hpp src/*.cpp)
add_executable(AxiomTests  ${AXIOM_TEST_SOURCES} )


target_include_directories(AxiomTests PUBLIC src)
target_include_directories(AxiomTests PUBLIC ../lib/doctest)

target_link_libraries(AxiomTests PUBLIC Axiom)