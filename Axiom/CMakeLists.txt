#axiom/axiom
project(Axiom)

file(GLOB_RECURSE AXIOM_SOURCES src/*.hpp src/*.cpp)

message(NOTICE ${AXIOM_PLATFORM_INCLUDE_DIRS})

if (BUILD_SHARED_LIBS)
	add_library(Axiom SHARED ${AXIOM_SOURCES} ${AXIOM_PLATFORM_SOURCES})
	target_compile_definitions(Axiom PUBLIC AX_SHARED=1)
	target_compile_definitions(Axiom PRIVATE AX_EXPORT=1)
else()
	add_library(Axiom STATIC ${AXIOM_SOURCES} ${AXIOM_PLATFORM_SOURCES})
	target_compile_definitions(Axiom PUBLIC AX_SHARED=0)
endif()

target_include_directories(Axiom PUBLIC src)
target_include_directories(Axiom PRIVATE ${AXIOM_PLATFORM_INCLUDE_DIRS})

target_link_libraries(Axiom PUBLIC glfw EnTT d3d12 dxgi dxguid d3dcompiler)
target_compile_definitions(Axiom PUBLIC NOMINMAX)