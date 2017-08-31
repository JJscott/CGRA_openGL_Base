
#pragma once

// project
#include "opengl.hpp"
#include "cgra/cgra_math.hpp"
#include "cgra/cgra_mesh.hpp"


// Teapot for displaying a textured mesh
//
class Teapot {
private:
	GLuint m_shader;
	GLuint m_texture;
	cgra::mesh m_mesh;

public:
	cgra::vec3 m_min;
	cgra::vec3 m_max;

	Teapot();
	void draw(const cgra::mat4 &view, const cgra::mat4 &proj, bool wireframe);

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

	// axis
	bool m_show_axis = false;
	GLuint m_axis_shader = 0;

	// aabb
	bool m_show_aabb = false;
	GLuint m_aabb_shader = 0;

	// geometry
	bool m_wireframe = false;
	Teapot m_test_teapot;

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