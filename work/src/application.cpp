

#include <iostream>
#include <string>

#include "application.hpp"

#include "cgra/cgra_gui.hpp"
#include "cgra/cgra_image.hpp"
#include "cgra/cgra_shader.hpp"


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
		shader = makeShaderProgramFromFile(
			//"330 core", {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER}, "work/res/shaders/simple_grey.glsl");
			"330 core", { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, "work/res/shaders/simple_texture.glsl"
			//"330 core", { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, "work/res/shaders/flat_model_normal_color.glsl"
		);
	}

	static GLuint texture = 0;
	if (texture == 0) {
		image<float, 4> img("work/res/textures/uv_texture.jpg");
		texture = img.make_texture();
	}

	static mesh geometry(GL_TRIANGLES);

	if (geometry.m_indices.empty()) {

		vector<vertex> vdata = {
			vertex(vec3(-1, -1, 0), vec3(0, 0, 1), vec2(0, 0)),
			vertex(vec3( 1, -1, 0), vec3(0, 0, 1), vec2(1, 0)),
			vertex(vec3( 1,  1, 0), vec3(0, 0, 1), vec2(1, 1)),
			vertex(vec3(-1,  1, 0), vec3(0, 0, 1), vec2(0, 1))
		};

		vector<unsigned int> idata = {
			0, 1, 2,
			3, 0, 2
		};

		geometry.m_vertices = vdata;
		geometry.m_indices = idata;

		geometry.reupload();
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

	glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, proj.data());
	glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, modelview.data());


	geometry.draw();

	
	if (m_show_axis) m_axis.draw(view, proj);


	glUseProgram(0); // Unbind our shader
	glBindVertexArray(0); // Unbind our geometry
}


void Application::renderGUI() {
	ImGui::Begin("Debug");
	ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat("Pitch", &m_pitch, -pi/2, pi/2);
	ImGui::SliderFloat("Yaw", &m_yaw, -pi, pi);
	ImGui::Checkbox("Show Axis", &m_show_axis);
	ImGui::End();
}



Axis::Axis() {
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

	m_shader = makeShaderProgramFromFile(
		"330 core",
		{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
		"work/res/shaders/flat_model_normal_color.glsl"
	);
}

void Axis::draw(const mat4 &view, const mat4 &proj) {
	mat4 modelview = view;

	glUseProgram(m_shader);
	glUniformMatrix4fv(glGetUniformLocation(m_shader, "uProjectionMatrix"), 1, false, proj.data());
	glUniformMatrix4fv(glGetUniformLocation(m_shader, "uModelViewMatrix"), 1, false, modelview.data());

	m_mesh.draw();
}