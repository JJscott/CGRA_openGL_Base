

#include <iostream>
#include <string>

#include "application.hpp"

#include "cgra/cgra_gui.hpp"
#include "cgra/cgra_image.hpp"
#include "cgra/cgra_shader.hpp"
#include "cgra/cgra_vao.hpp"


using namespace std;
using namespace cgra;


Application::Application() {
	// TODO
}


void Application::cursorPosCallback(double xpos, double ypos) {
	// do something
	if (m_leftMouseDown) {
		m_pitch = clamp(m_pitch + radians(ypos - m_mousePosition.y), -pi / 2, pi/2);
		m_yaw += radians(xpos - m_mousePosition.x);
		if (m_yaw > pi) m_yaw -= 2*pi;
		else if (m_yaw < -pi) m_yaw += 2*pi;
	}
	m_mousePosition = vec2(xpos, ypos);
}


void Application::mouseButtonCallback(int button, int action, int mods) {
	// do something
	if (button == GLFW_MOUSE_BUTTON_LEFT)
		m_leftMouseDown = (action == GLFW_PRESS);
}


void Application::scrollCallback(double xoffset, double yoffset) {
	// do something
}


void Application::keyCallback(int key, int scancode, int action, int mods) {
	// do something
}


void Application::charCallback(unsigned int c) {
	// do something
}


void Application::render(int width, int height) {
	static GLuint shader = 0;
	if (shader == 0) {
		shader = makeShaderProgramFromFile("330 core",
			//{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER}, "work/res/shaders/simple_grey.glsl");
		{ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, "work/res/shaders/simple_texture.glsl");
	}

	static GLuint texture = 0;
	if (texture == 0) {
		image<float, 4> img("work/res/textures/uv_texture.jpg");
		texture = img.make_texture();
	}

	static SimpleVAO *geometry = nullptr;
	if (geometry == nullptr) {
		geometry = new SimpleVAO();
		geometry->begin(GL_TRIANGLES);

		geometry->set_normal(0, 0, 1);

		// first triangle
		geometry->set_texcoord(0, 0);
		geometry->add_vertex(-1, -1, 0);
		geometry->set_texcoord(1, 0);
		geometry->add_vertex(1, -1, 0);
		geometry->set_texcoord(0, 1);
		geometry->add_vertex(-1, 1, 0);

		// second triangle
		geometry->set_texcoord(1, 1);
		geometry->add_vertex(1, 1, 0);
		geometry->set_texcoord(1, 0);
		geometry->add_vertex(1, -1, 0);
		geometry->set_texcoord(0, 1);
		geometry->add_vertex(-1, 1, 0);

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
	mat4 proj = perspective<mat4>(1.0, float(width) / height, 0.1, 100.0);
	mat4 view = translate3<mat4>(0, 0, -5) * rotate3x<mat4>(m_pitch) * rotate3y<mat4>(m_yaw);
	mat4 model(1); // identity

	mat4 modelview = view * model;

	glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, modelview.data());
	glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, view.data());


	geometry->draw();


	glUseProgram(0); // Unbind our shader
	glBindVertexArray(0); // Unbind our geometry
}


void Application::renderGUI() {
	ImGui::Begin("Debug");
	ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat("Pitch", &m_pitch, -pi/2, pi/2);
	ImGui::SliderFloat("Yaw", &m_yaw, -pi, pi);
	ImGui::End();
}