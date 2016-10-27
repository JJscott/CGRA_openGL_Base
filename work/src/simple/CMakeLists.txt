
# Source files
set(sources	
	"simple_gui.hpp"
	"simple_gui.cpp"
	"simple_image.hpp"
	"simple_shader.hpp"
	"simple_vao.hpp"
	"simple_vao.cpp"
)

# Add these sources to the project target
target_relative_sources(${CGRA_PROJECT} ${sources})
