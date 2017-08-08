
#pragma once

// project
#include "opengl.hpp"
#include "cgra/cgra_math.hpp"
#include "cgra/cgra_mesh.hpp"


class Axis {
private:
	GLuint m_shader;
	cgra::mesh m_mesh;
public:
	Axis();
	void draw(const cgra::mat4 &view, const cgra::mat4 &proj);
};

class TestQuad {
private:
	GLuint m_shader;
	GLuint m_texture;
	cgra::mesh m_mesh;
public:
	TestQuad();
	void draw(const cgra::mat4 &view, const cgra::mat4 &proj);

};

class Application {
private:
	float m_pitch = 0;
	float m_yaw = 0;

	bool m_leftMouseDown = false;
	cgra::vec2 m_mousePosition;

	bool m_show_axis = false;
	Axis m_axis;

	TestQuad m_test_quad;


public:

	Application();

	// disable copy constructors (for safety)
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	void cursorPosCallback(double xpos, double ypos);
	void mouseButtonCallback(int button, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
	void keyCallback(int key, int scancode, int action, int mods);
	void charCallback(unsigned int c);

	void render(int width, int height);
	void renderGUI();
};