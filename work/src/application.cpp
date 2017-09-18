
// std
#include <iostream>
#include <string>

// project
#include "application.hpp"
#include "cgra/cgra_gui.hpp"
#include "cgra/cgra_image.hpp"
#include "cgra/cgra_shader.hpp"
#include "cgra/cgra_wavefront.hpp"


using namespace std;
using namespace cgra;


Application::Application() {
	// compile axis shader
	shader_builder prog;
	prog.set_shader(GL_VERTEX_SHADER, "work/res/shaders/axis.glsl");
	prog.set_shader(GL_GEOMETRY_SHADER, "work/res/shaders/axis.glsl");
	prog.set_shader(GL_FRAGMENT_SHADER, "work/res/shaders/axis.glsl");
	m_axis_shader = prog.build();
}


void Application::cursorPosCallback(double xpos, double ypos) {
	if (m_leftMouseDown) {
		vec2 whsize = m_windowsize / 2.0f;

		// clamp the pitch to [-pi/2, pi/2]
		m_pitch += float(acos(clamp((m_mousePosition.y - whsize.y) / whsize.y, -1.0f, 1.0f))
			- acos(clamp((ypos - whsize.y) / whsize.y, -1.0f, 1.0f)));
		m_pitch = float(clamp(m_pitch, -pi / 2, pi / 2));

		// wrap the yaw to [-pi, pi]
		m_yaw += float(acos(clamp((m_mousePosition.x - whsize.x) / whsize.x, -1.0f, 1.0f))
			- acos(clamp((xpos - whsize.x) / whsize.x, -1.0f, 1.0f)));
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
	(void) xoffset; // currently un-used

	// logrithmic zoom
	float d = float(100 * pow(pow(m_distance / 100, 0.5) - (yoffset/20), 2.0));
	if (d == d && !isinf(d))
		m_distance = clamp(d, 0, 100);
}


void Application::keyCallback(int key, int scancode, int action, int mods) {
	(void) key, scancode, action, mods; // currently un-used
}


void Application::charCallback(unsigned int c) {
	(void) c; // currently un-used
}


void Application::render(int width, int height) {
	// update window size
	m_windowsize = vec2(width, height);
	
	// ensure we draw to the entire 
	glViewport(0, 0, width, height);

	// clear the back-buffer to a solid grey/blueish background
	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// enable flags for normal/forward rendering
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

	// calculate the projection and view matrix
	mat4 proj = perspective(1.0, float(width) / height, 0.1, 1000.0);
	mat4 view = translate3(0, 0, -m_distance) * rotate3x(m_pitch) * rotate3y(m_yaw);

	// draw axis
	if (m_show_axis) {
		// load shader and variables
		glUseProgram(m_axis_shader);
		glUniformMatrix4fv(glGetUniformLocation(m_axis_shader, "uProjectionMatrix"), 1, false, proj.data());
		glUniformMatrix4fv(glGetUniformLocation(m_axis_shader, "uModelViewMatrix"), 1, false, view.data());
		// the shader requires 6 instances to draw all 6 lines for the axes
		// geometry is created inside the shader
		draw_dummy(6);
	}

	// draw the teapot
	m_test_teapot.draw(view, proj);
}


void Application::renderGUI() {

	// setup window
	ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiSetCond_Once);
	ImGui::Begin("Camera", 0);

	// display current camera parameters
	ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat("Pitch", &m_pitch, float(-pi/2), float(pi/2), "%.2f");
	ImGui::SliderFloat("Yaw", &m_yaw, float(-pi), float(pi), "%.2f");
	ImGui::SliderFloat("Distance", &m_distance, 0, 100, "%.2f", 2.0f);

	// extra drawing parameters
	ImGui::Checkbox("Show Axis", &m_show_axis);
	ImGui::SameLine();
	ImGui::Checkbox("Show Wireframe", &m_test_teapot.m_show_wireframe);
	ImGui::Checkbox("Show AABB", &m_test_teapot.m_show_abb);
	ImGui::SameLine();
	ImGui::Checkbox("Show Texture", &m_test_teapot.m_show_texture);

	// finish creating window
	ImGui::End();
}


Teapot::Teapot() {

	// compile grey shader
	shader_builder prog;
	prog.set_shader(GL_VERTEX_SHADER, "work/res/shaders/simple_grey.glsl");
	prog.set_shader(GL_FRAGMENT_SHADER, "work/res/shaders/simple_grey.glsl");
	m_grey_shader = prog.build();

	// compile texture shader
	prog = shader_builder();
	prog.set_shader(GL_VERTEX_SHADER, "work/res/shaders/simple_texture.glsl");
	prog.set_shader(GL_FRAGMENT_SHADER, "work/res/shaders/simple_texture.glsl");
	m_texture_shader = prog.build();

	// compile aabb shader
	prog = shader_builder();
	prog.set_shader(GL_VERTEX_SHADER, "work/res/shaders/aabb.glsl");
	prog.set_shader(GL_GEOMETRY_SHADER, "work/res/shaders/aabb.glsl");
	prog.set_shader(GL_FRAGMENT_SHADER, "work/res/shaders/aabb.glsl");
	m_aabb_shader = prog.build();

	// load texture
	//image<float, 4> img("work/res/textures/uv_texture.jpg");
	image<float, 4> img("work/res/textures/checkerboard.jpg");
	m_texture = img.upload_texture();

	// load mesh
	mesh_builder md = cgra::load_wavefront_data("work/res/assets/teapot.obj");
	m_mesh = md.build(m_mesh);

	// compute min/max
	if (md.vertices().size()) {
		m_min = m_max = md.vertices()[0].pos;
		for (auto &v : md.vertices()) {
			m_min = min(m_min, v.pos);
			m_max = max(m_max, v.pos);
		}
	}
}


void Teapot::draw(const cgra::mat4 &view, const cgra::mat4 &proj) {

	// create the model/view matrix
	mat4 modelview = view;


	// draw the AABB
	if (m_show_abb) {
		// load shader and variables
		glUseProgram(m_aabb_shader);
		glUniformMatrix4fv(glGetUniformLocation(m_aabb_shader, "uProjectionMatrix"), 1, false, proj.data());
		glUniformMatrix4fv(glGetUniformLocation(m_aabb_shader, "uModelViewMatrix"), 1, false, modelview.data());
		glUniform3fv(glGetUniformLocation(m_aabb_shader, "uMin"), 1, m_min.data());
		glUniform3fv(glGetUniformLocation(m_aabb_shader, "uMax"), 1, m_max.data());
		// the shader requires 12 instances to draw all 12 lines for the aabb
		// geometry is created inside the shader
		draw_dummy(12);
	}


	// load shader and variables
	GLuint shader = (m_show_texture) ? m_texture_shader : m_grey_shader;
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, proj.data());
	glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, modelview.data());

	// load texture
	glActiveTexture(GL_TEXTURE0); // Set the location for binding the texture
	glBindTexture(GL_TEXTURE_2D, m_texture); // Bind the texture
	glUniform1i(glGetUniformLocation(shader, "uTexture0"), 0);  // Set our sampler (texture0) to use GL_TEXTURE0 as the source

	// draw
	m_mesh.draw(m_show_wireframe);
}