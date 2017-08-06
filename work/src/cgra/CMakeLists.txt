
# Source files
set(sources	
	"cgra_gui.hpp"
	"cgra_gui.cpp"
	
	"cgra_image.hpp"

	"cgra_math.hpp"

	"cgra_shader.hpp"

	"cgra_vao.hpp"
	"cgra_vao.cpp"
)

# Add these sources to the project target
target_relative_sources(${CGRA_PROJECT} ${sources})
