
#pragma once

#include "opengl.hpp"

#include "cgra/cgra_math.hpp"

class Application {
private:
	float m_pitch = 0;
	float m_yaw = 0;

	bool m_leftMouseDown = false;
	cgra::vec2 m_mousePosition;

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