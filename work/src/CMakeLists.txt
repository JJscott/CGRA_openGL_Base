
# Source files
# list your source files (.cpp) here
SET(sources
	"main.cpp"
	"cgra_math.hpp"
	"opengl.hpp"
)


# Add executable target and link libraries
add_executable(${CGRA_PROJECT} ${headers} ${sources})


# Sources from subdirectories
# list your subdirectories here
add_subdirectory(simple)


# Set source groups (helper method)
target_source_group_tree(${CGRA_PROJECT})

# Link usage requirements
target_link_libraries(${CGRA_PROJECT} PRIVATE glew glfw ${GLFW_LIBRARIES})
target_link_libraries(${CGRA_PROJECT} PRIVATE stb imgui)

# For experimental <filesystem>
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_link_libraries(${CGRA_PROJECT} PRIVATE -lstdc++fs)
endif()