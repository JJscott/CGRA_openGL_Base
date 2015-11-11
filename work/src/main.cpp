//---------------------------------------------------------------------------
//
// Copyright (c) 2015 Taehyun Rhee, Joshua Scott, Ben Allen
//
// This software is provided 'as-is' for assignment of COMP308 in ECS,
// Victoria University of Wellington, without any express or implied warranty. 
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "cgra_math.hpp"
#include "openGL.hpp"
#include "simpleShader.hpp"
#include "simpleGUI.hpp"
#include "geometry.hpp"

using namespace std;
using namespace cgra;

GLFWwindow* window;

void mouseButtonCallback(GLFWwindow *win, int button, int action, int mods) {
	SimpleGUI::mouseButtonCallback(win, button, action, mods);
}


void scrollCallback(GLFWwindow *win, double xoffset, double yoffset) {
	SimpleGUI::scrollCallback(win, xoffset, yoffset);
}


void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
	SimpleGUI::keyCallback(win, key, scancode, action, mods);
}


void charCallback(GLFWwindow *win, unsigned int c) {
	SimpleGUI::charCallback(win, c);
}

void render() {

}

void renderGUI() {
	
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

	// Get the version for later
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

		// Grey/Blueish background
		glClearColor(0.3f,0.3f,0.4f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// RENDER HERE !!!
		//



		// Draw imgui
		SimpleGUI::newFrame();

		ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Another Window");
		ImGui::Text("Hello");
		ImGui::End();
		
		SimpleGUI::render();

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	SimpleGUI::shutdown();
	glfwTerminate();

}