project(AxiomHeaderTool)

if (UNIX)
	set (show_contents_prog cat)
elseif (WIN32)
	set (show_contents_prog type)
endif()

#execute_process(COMMAND cmake -DCMAKE_BUILD_TYPE=Debug -G Ninja -B ${PROJECT_SOURCE_DIR}/build OUTPUT_VARIABLE file_contents RESULT_VARIABLE file_result WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
#message("path ${PROJECT_SOURCE_DIR}")
#message("yo ${file_contents} - ${file_result}")

file(GLOB_RECURSE PROJECT_SOURCE src/*.hpp src/*.cpp src/*.h src/*.c)

add_executable(AxiomHeaderTool ${PROJECT_SOURCE})