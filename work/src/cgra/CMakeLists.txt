
# Source files
set(sources	
	"cgra_gui.hpp"
	"cgra_gui.cpp"
	
	"cgra_image.hpp"

	"cgra_math.hpp"
	"cgra_math.natvis"

	"cgra_mesh.hpp"
	"cgra_mesh.cpp"

	"cgra_shader.hpp"
	"cgra_shader.cpp"

	"cgra_util.hpp"
)

# Add these sources to the project target
target_relative_sources(${CGRA_PROJECT} ${sources})
