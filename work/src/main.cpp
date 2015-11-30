
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "cgra_math.hpp"
#include "opengl.hpp"
#include "simple_gui.hpp"
#include "simple_image.hpp"
#include "simple_shader.hpp"

using namespace std;
using namespace cgra;

GLFWwindow* window;


void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {

}


void mouseButtonCallback(GLFWwindow *win, int button, int action, int mods) {
	SimpleGUI::mouseButtonCallback(win, button, action, mods);
	if(ImGui::IsMouseHoveringAnyWindow()) return; // block input with gui 

}


void scrollCallback(GLFWwindow *win, double xoffset, double yoffset) {
	SimpleGUI::scrollCallback(win, xoffset, yoffset);
	if(ImGui::IsMouseHoveringAnyWindow()) return; // block input with gui

}


void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
	SimpleGUI::keyCallback(win, key, scancode, action, mods);
	if(ImGui::IsAnyItemActive()) return; // block input with gui

}


void charCallback(GLFWwindow *win, unsigned int c) {
	SimpleGUI::charCallback(win, c);
	if(ImGui::IsAnyItemActive()) return; // block input with gui

}


void render() {

}


void renderGUI() {
	ImGui::Begin("Debug");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}


//Main program
// 
int main() {
// int main(int argc, char **argv) {

	// Initialize the GLFW library
	if (!glfwInit()) {
		cerr << "Error: Could not initialize GLFW" << endl;
		abort(); // Unrecoverable error
	}

	// Get the version for GLFW for later
	int glfwMajor, glfwMinor, glfwRevision;
	glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
	if (!window) {
		cerr << "Error: Could not create GLFW window" << endl;
		abort(); // Unrecoverable error
	}

	// Make the window's context current.
	// If we have multiple windows we will need to switch contexts
	glfwMakeContextCurrent(window);


	// Initialize GLEW
	// must be done after creating GL context (glfwCreateWindow in this case)
	GLenum err = glewInit();
	if (GLEW_OK != err) { // Problem: glewInit failed, something is seriously wrong.
		cerr << "Error: " << glewGetErrorString(err) << endl;
		abort(); // Unrecoverable error
	}


	// Print out our OpenGL verisions
	cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	cout << "Using GLFW " << glfwMajor << "." << glfwMinor << "." << glfwRevision << endl;


	// Attach input callbacks to window
	// Should not be done if we want imgui to control input
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCharCallback(window, charCallback);


	// Initialize IMGUI
	// Second argument is true if we dont need to use GLFW bindings for input
	// if set to false we must manually call the SimpleGUI callbacks when we
	// process the input.
	// if (!SimpleGUI::init(window, true)) {
	if (!SimpleGUI::init(window, false)) {
		cerr << "Error: Could not initialize IMGUI" << endl;
		abort();
	}


	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {

		// Make sure we draw to the WHOLE window
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// Grey/Blueish background
		glClearColor(0.3f,0.3f,0.4f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Main Render
		render();

		// GUI Render on top
		SimpleGUI::newFrame();
		renderGUI();
		SimpleGUI::render();

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	SimpleGUI::shutdown();
	glfwTerminate();

}