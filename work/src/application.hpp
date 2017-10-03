
#pragma once

// project
#include "opengl.hpp"
#include "cgra/cgra_math.hpp"
#include "cgra/cgra_mesh.hpp"


// Teapot for displaying a textured mesh
//
class Teapot {
private:
	// shaders
	GLuint m_texture_shader = 0;
	GLuint m_grey_shader = 0;
	GLuint m_aabb_shader = 0;

	// data
	GLuint m_texture;
	cgra::mesh m_mesh;
	cgra::vec3 m_min;
	cgra::vec3 m_max;

public:
	bool m_show_abb = false;
	bool m_show_texture = false;
	bool m_show_wireframe = false;

	Teapot();
	void draw(const cgra::mat4 &view, const cgra::mat4 &proj);
};


// Main application class
//
class Application {
private:
	// window
	cgra::vec2 m_windowsize;
	GLFWwindow *m_window;

	// oribital camera
	float m_pitch = 0;
	float m_yaw = 0;
	float m_distance = 50;

	// last input
	bool m_leftMouseDown = false;
	cgra::vec2 m_mousePosition;

	// axis
	bool m_show_axis = false;
	GLuint m_axis_shader = 0;

	// geometry
	Teapot m_test_teapot;

public:
	// setup
	Application(GLFWwindow *);

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
	void render();
	void renderGUI();
};