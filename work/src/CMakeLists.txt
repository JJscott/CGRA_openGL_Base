#
# CGRA Project
# If you add new source files, you must list them here
#

# TODO list your header files (.hpp) here
SET(headers
	"cgra_math.hpp"
	"opengl.hpp"
	"simple_gui.hpp"
	"simple_image.hpp"
	"simple_shader.hpp"
)


# TODO list your source files (.cpp) here
SET(sources
	"main.cpp"
	"simple_gui.cpp"
)

SET(test_sources
	"cgra_math.hpp"
	"cgra_math_test.cpp"
)

# Add executable target and link libraries
# You do not need to touch this
add_executable(${CGRA_PROJECT} ${headers} ${sources})
target_link_libraries(${CGRA_PROJECT} PRIVATE glew glfw ${GLFW_LIBRARIES})
target_link_libraries(${CGRA_PROJECT} PRIVATE stb imgui)


add_executable(cgra_math_tests ${test_sources})