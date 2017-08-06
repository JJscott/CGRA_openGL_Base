
#pragma once

#include <imgui.h>

#include "../opengl.hpp"

namespace cgra {
	namespace gui {

		// callback functions required to forward input to ImGui
		//

		void mouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/);
		void scrollCallback(GLFWwindow*, double /*xoffset*/, double yoffset);
		void keyCallback(GLFWwindow*, int key, int /*scancode*/, int action, int mods);
		void charCallback(GLFWwindow*, unsigned int c);


		// Helper functions to setup, run and shutdown ImGui
		//

		bool init(GLFWwindow* window, bool install_callbacks=false);
		void newFrame();
		void render();
		void shutdown();
	}
}