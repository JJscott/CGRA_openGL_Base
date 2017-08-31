
// std
#include <iostream>
#include <string>

// project
#include "application.hpp"
#include "cgra/cgra_gui.hpp"
#include "cgra/cgra_image.hpp"
#include "cgra/cgra_shader.hpp"


using namespace std;
using namespace cgra;


Application::Application() { }


void Application::cursorPosCallback(double xpos, double ypos) {
	if (m_leftMouseDown) {
		// clamp the pitch to [-pi/2, pi/2]
		m_pitch = float(clamp(m_pitch + radians(ypos - m_mousePosition.y), -pi / 2, pi / 2));

		// wrap the yaw to [-pi, pi]
		m_yaw += float(radians(xpos - m_mousePosition.x));
		if (m_yaw > pi) m_yaw -= float(2 * pi);
		else if (m_yaw < -pi) m_yaw += float(2 * pi);
	}

	// updated mouse position
	m_mousePosition = vec2(xpos, ypos);
}


void Application::mouseButtonCallback(int button, int action, int mods) {
	(void) mods; // currently un-used

	// capture is left-mouse down
	if (button == GLFW_MOUSE_BUTTON_LEFT)
		m_leftMouseDown = (action == GLFW_PRESS); // only other option is GLFW_RELEASE
}


void Application::scrollCallback(double xoffset, double yoffset) {
	(void) xoffset, yoffset; // currently un-used
}


void Application::keyCallback(int key, int scancode, int action, int mods) {
	(void) key, scancode, action, mods; // currently un-used
}


void Application::charCallback(unsigned int c) {
	(void) c; // currently un-used
}


void Application::render(int width, int height) {
	
	// ensure we draw to the entire 
	glViewport(0, 0, width, height);

	// clear the back-buffer to a solid grey/blueish background
	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// enable flags for normal/forward rendering
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

	// calculate the projection and view matrix
	mat4 proj = perspective(1.0, float(width) / height, 0.1, 100.0);
	mat4 view = translate3(0, 0, -5) * rotate3x(m_pitch) * rotate3y(m_yaw);

	// draw geometry
	if (m_show_axis) m_axis.draw(view, proj);
	m_test_quad.draw(view, proj);
}


void Application::renderGUI() {

	// setup window
	ImGui::SetNextWindowSize(ImVec2(280, 120), ImGuiSetCond_Once);
	ImGui::Begin("Camera", 0);

	// display current camera parameters
	ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat("Pitch", &m_pitch, float(-pi/2), float(pi/2));
	ImGui::SliderFloat("Yaw", &m_yaw, float(-pi), float(pi));
	ImGui::Checkbox("Show Axis", &m_show_axis);

	// finish creating window
	ImGui::End();
}



Axis::Axis() {

	// compile shader
	shader_program prog;
	prog.set_shader(GL_VERTEX_SHADER, "work/res/shaders/flat_model_normal_color.glsl");
	prog.set_shader(GL_FRAGMENT_SHADER, "work/res/shaders/flat_model_normal_color.glsl");
	m_shader = prog.compile();

	// load mesh
	m_mesh.m_mode = GL_LINES;
	m_mesh.m_vertices = {
		vertex(vec3(0, 0, 0), vec3(1, 0, 0)),
		vertex(vec3(1e+15, 0, 0), vec3(1, 0, 0)),
		vertex(vec3(-1e+15, 0, 0), vec3(1, 0, 0)),
		vertex(vec3(0, 0, 0), vec3(0, 1, 0)),
		vertex(vec3(0, 1e+15, 0), vec3(0, 1, 0)),
		vertex(vec3(0, -1e+15, 0), vec3(0, 1, 0)),
		vertex(vec3(0, 0, 0), vec3(0, 0, 1)),
		vertex(vec3(0, 0, 1e+15), vec3(0, 0, 1)),
		vertex(vec3(0, 0, -1e+15), vec3(0, 0, 1))
	};
	m_mesh.m_indices = {
		0, 1,   0, 2,
		3, 4,   3, 5,
		6, 7,   6, 8
	};
}


void Axis::draw(const mat4 &view, const mat4 &proj) {

	// create the model/view matrix
	mat4 modelview = view;

	// load shader and variables
	glUseProgram(m_shader);
	glUniformMatrix4fv(glGetUniformLocation(m_shader, "uProjectionMatrix"), 1, false, proj.data());
	glUniformMatrix4fv(glGetUniformLocation(m_shader, "uModelViewMatrix"), 1, false, modelview.data());

	// draw
	m_mesh.draw();
}



TestQuad::TestQuad() {

	// compile shader
	shader_program prog;
	prog.set_shader(GL_VERTEX_SHADER, "work/res/shaders/simple_texture.glsl");
	prog.set_shader(GL_FRAGMENT_SHADER, "work/res/shaders/simple_texture.glsl");
	m_shader = prog.compile();

	// load texture
	image<float, 4> img("work/res/textures/uv_texture.jpg");
	m_texture = img.upload_texture();

	// load mesh
	m_mesh.m_mode = GL_TRIANGLES;
	m_mesh.m_vertices = {
		vertex(vec3(-1, -1, 0), vec3(0, 0, 1), vec2(0, 0)),
		vertex(vec3(1, -1, 0), vec3(0, 0, 1), vec2(1, 0)),
		vertex(vec3(1,  1, 0), vec3(0, 0, 1), vec2(1, 1)),
		vertex(vec3(-1,  1, 0), vec3(0, 0, 1), vec2(0, 1))
	};
	m_mesh.m_indices = {
		0, 1, 2,
		3, 0, 2
	};
	m_mesh.reupload();
}


void TestQuad::draw(const cgra::mat4 &view, const cgra::mat4 &proj) {

	// create the model/view matrix
	mat4 modelview = view;

	// load shader and variables
	glUseProgram(m_shader);
	glUniformMatrix4fv(glGetUniformLocation(m_shader, "uProjectionMatrix"), 1, false, proj.data());
	glUniformMatrix4fv(glGetUniformLocation(m_shader, "uModelViewMatrix"), 1, false, modelview.data());

	// load texture
	glActiveTexture(GL_TEXTURE0); // Set the location for binding the texture
	glBindTexture(GL_TEXTURE_2D, m_texture); // Bind the texture
	glUniform1i(glGetUniformLocation(m_shader, "uTexture0"), 0);  // Set our sampler (texture0) to use GL_TEXTURE0 as the source

	// draw
	m_mesh.draw();
}