
#pragma once

// project
#include "opengl.hpp"
#include "cgra/cgra_math.hpp"
#include "cgra/cgra_mesh.hpp"

// Helper class to draw an axis
//
class Axis {
private:
	GLuint m_shader;
	cgra::mesh m_mesh;
public:
	Axis();
	void draw(const cgra::mat4 &view, const cgra::mat4 &proj);
};

// Quad for displaying a textured mesh
//
class Teapot {
private:
	GLuint m_shader;
	GLuint m_texture;
	cgra::mesh m_mesh;
public:
	Teapot();
	void draw(const cgra::mat4 &view, const cgra::mat4 &proj);

};

// Main application class
//
class Application {
private:
	// oribital camera
	float m_pitch = 0;
	float m_yaw = 0;
	float m_distance = 50;

	// last input
	bool m_leftMouseDown = false;
	cgra::vec2 m_mousePosition;

	// geometry
	bool m_show_axis = false;
	Axis m_axis;
	Teapot m_test_quad;

public:
	// setup
	Application();

	// disable copy constructors (for safety)
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	// input callbacks
	void cursorPosCallback(double xpos, double ypos);
	void mouseButtonCallback(int button, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
	void keyCallback(int key, int scancode, int action, int mods);
	void charCallback(unsigned int c);

	// rendering callbacks (every frame)
	void render(int width, int height);
	void renderGUI();
};