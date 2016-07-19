
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include "cgra_math.hpp"
#include "opengl.hpp"
#include "simple_gui.hpp"
#include "simple_image.hpp"
#include "simple_shader.hpp"
#include "simple_vao.hpp"

using namespace std;
using namespace cgra;

GLFWwindow* window;

float pitch = 0;
float yaw = 0;

bool leftMouseDown = false;
vec2 mousePosition;


void cursorPosCallback(GLFWwindow *, double xpos, double ypos) {
	if (leftMouseDown) {
		yaw += radians(mousePosition.x - xpos);
		pitch += radians(mousePosition.y - ypos);
	}
	mousePosition = vec2(xpos, ypos);
}


void mouseButtonCallback(GLFWwindow *win, int button, int action, int mods) {
	SimpleGUI::mouseButtonCallback(win, button, action, mods);
	if(ImGui::IsMouseHoveringAnyWindow()) return; // block input with gui 

	if (button == GLFW_MOUSE_BUTTON_LEFT)
		leftMouseDown = (action == GLFW_PRESS);
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


void render(int width, int height) {
	static GLuint shader = 0;
	if (shader == 0) {
		shader = makeShaderProgramFromFile("330 core",
			//{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER}, "work/res/shaders/simple_grey.glsl");
			{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER}, "work/res/shaders/simple_texture.glsl");
	}

	static GLuint texture = 0;
	if (texture == 0) {
		image<4> tex("work/res/textures/checkerboard.jpg");
		texture = tex.make_texture();
	}

	static SimpleVAO *geometry = nullptr;
	if (geometry == nullptr) {
		geometry = new SimpleVAO();
		geometry->begin(GL_TRIANGLES);

		geometry->normal3f(0,0,1);

		// first triangle
		geometry->texCoord2f(0,0);
		geometry->vertex3f(-1,-1,0);
		geometry->texCoord2f(1,0);
		geometry->vertex3f(1,-1,0);
		geometry->texCoord2f(0,1);
		geometry->vertex3f(-1,1,0);

		// second triangle
		geometry->texCoord2f(1,1);
		geometry->vertex3f(1,1,0);
		geometry->texCoord2f(1,0);
		geometry->vertex3f(1,-1,0);
		geometry->texCoord2f(0,1);
		geometry->vertex3f(-1,1,0);

		geometry->end();
	}


	glEnable(GL_DEPTH_TEST); // Enable flags for normal rendering
	glDepthFunc(GL_LESS);

	glUseProgram(shader); // Use the shader we made

	glActiveTexture(GL_TEXTURE0); // Set the location for binding the texture
	glBindTexture(GL_TEXTURE_2D, texture); // Bind the texture
	// Set our sampler (texture0) to use GL_TEXTURE0 as the source
	glUniform1i(glGetUniformLocation(shader, "uTexture0"), 0);


	// Set up the matricies / uniforms
	// 
	mat4 proj = mat4::perspectiveProjection(1.0, float(width)/height, 0.1, 100.0);
	mat4 view = mat4::translate(0,0,-5) * mat4::rotate_x(-pitch) * mat4::rotate_y(-yaw);
	glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, proj.data());
	glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, view.data());


	geometry->draw();


	glUseProgram(0); // Unbind our shader
	glBindVertexArray(0); // Unbind our geometry
}


void renderGUI() {
	ImGui::Begin("Debug");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

}


// Forward decleration for cleanliness
void APIENTRY debugCallback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, GLvoid*);


//Main program
// 
int main() {
// int main(int argc, char **argv) {

	// Initialize the GLFW library
	if (!glfwInit()) {
		cerr << "Error: Could not initialize GLFW" << endl;
		abort(); // Unrecoverable error
	}

	//// Force OpenGL to create a 3.3 core context
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//// Disallow legacy functionality (helps OS X work)
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Get the version for GLFW for later
	int glfwMajor, glfwMinor, glfwRevision;
	glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);

	// Request a debug context so we get debug callbacks.
	// Remove this for possible GL performance increases.
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

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
	// must be done after making a GL context current (glfwMakeContextCurrent in this case)
	glewExperimental = GL_TRUE; // required for full GLEW functionality for OpenGL 3.0+
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


	// Enable GL_ARB_debug_output if available. Not necessary, just helpful
	if (glfwExtensionSupported("GL_ARB_debug_output")) {
		// This allows the error location to be determined from a stacktrace
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		// Setup up the callback
		glDebugMessageCallbackARB(debugCallback, nullptr);
		glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
		cout << "GL_ARB_debug_output callback installed" << endl;
	} else {
		cout << "GL_ARB_debug_output not available. No worries." << endl;
	}


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
		render(width, height);

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



//
// Fancy debug stuff
//

// function to translate source to string
const char * getStringForSource(GLenum source) {
	switch(source) {
	case GL_DEBUG_SOURCE_API: 
		return "API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return "Window System";
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return "Shader Compiler";
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return "Third Party";
	case GL_DEBUG_SOURCE_APPLICATION:
		return "Application";
	case GL_DEBUG_SOURCE_OTHER:
		return "Other";
	default:
		return "n/a";
	}
}

// function to translate severity to string
const char * getStringForSeverity(GLenum severity) {
	switch(severity) {
	case GL_DEBUG_SEVERITY_HIGH: 
		return "High";
	case GL_DEBUG_SEVERITY_MEDIUM:
		return "Medium";
	case GL_DEBUG_SEVERITY_LOW:
		return "Low";
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		return "None";
	default:
		return "n/a";
	}
}

// function to translate type to string
const char * getStringForType(GLenum type) {
	switch(type) {
	case GL_DEBUG_TYPE_ERROR: 
		return "Error";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return "Deprecated Behaviour";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return "Undefined Behaviour";
	case GL_DEBUG_TYPE_PORTABILITY:
		return "Portability";
	case GL_DEBUG_TYPE_PERFORMANCE:
		return "Performance";
	case GL_DEBUG_TYPE_OTHER:
		return "Other";
	default:
		return "n/a";
	}
}

// actually define the function
void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message, GLvoid*) {
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

	// nvidia: avoid debug spam about attribute offsets
	if (id == 131076) return;

	cerr << "GL [" << getStringForSource(source) << "] " << getStringForType(type) << ' ' << id << " : ";
	cerr << message << " (Severity: " << getStringForSeverity(severity) << ')' << endl;

	if (type == GL_DEBUG_TYPE_ERROR_ARB) throw runtime_error("GL Error: "s + message);
}
