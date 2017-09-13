
#pragma once

// std
#include <vector>

// project
#include "cgra_math.hpp"
#include <opengl.hpp>

namespace cgra {

	// A data structure for holding buffer IDs 
	// and other information related to drawing
	// also has helper functions for drawing, and
	// deleting the buffers (for cleanup)
	struct mesh {
		// GL buffer ids
		GLuint m_vao = 0;
		GLuint m_vbo = 0;
		GLuint m_ibo = 0;

		// index count (how much to draw)
		int m_index_count;

		// mode to draw in
		GLenum m_mode = 0;

		// calls the draw function and optionally sets the 
		// draw mode to lines instead of polygons
		void draw(bool wireframe = false);

		// deallocates the vertex buffer and vertex array objects
		void destroy();
	};


	struct vertex {
		vec3 pos;
		vec3 norm;
		vec2 uv;
		vertex(vec3 p = {}, vec3 n = {}, vec2 t = {})
			: pos(p), norm(n), uv(t){ }
		vertex(vec3 p, vec2 t)
			: pos(p), norm(0, 0, 1), uv(t) { }
	};


	// Mesh builder object. Used to create an Mesh
	// by taking vertex and index information
	// and uploading them to OpenGL
	class mesh_builder {
	private:
		std::vector<vertex> m_vertices;
		std::vector<unsigned int> m_indices;
		GLenum m_mode;

	public:
		mesh_builder(
			const std::vector<vertex> &vertices = {},
			const std::vector<unsigned int> &indices = {},
			GLenum mode = GL_TRIANGLES
		);

		std::vector<vertex> & vertices() { return m_vertices; }
		const std::vector<vertex> & vertices() const { return m_vertices; }

		std::vector<unsigned int> & indices() { return m_indices; }
		const std::vector<unsigned int> & indices() const { return m_indices; }

		GLenum & mode() { return m_mode; }
		const GLenum & mode() const { return m_mode; }

		mesh build(mesh m = {});
	};

}

