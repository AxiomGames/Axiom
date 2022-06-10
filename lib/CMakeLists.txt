#------------------------------
# GLFW
# Desktop Window Library
#------------------------------
if(NOT TARGET glfw AND NOT PLATFORM_ANDROID)
	set(GLFW_INSTALL OFF)
	set(GLFW_BUILD_EXAMPLES OFF)
	set(GLFW_BUILD_TESTS OFF)
	set(GLFW_BUILD_DOCS OFF)
	set(GLFW_USE_HYBRID_HPG ON)
	add_subdirectory(glfw)
	set_property(TARGET glfw PROPERTY FOLDER "Libs")
endif()