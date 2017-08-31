
#pragma once

// std
#include <vector>

// project
#include "cgra_math.hpp"
#include <opengl.hpp>

namespace cgra {

	struct mesh {
		// GL buffer ids
		GLuint m_vao = 0;
		GLuint m_vbo = 0;
		GLuint m_ibo = 0;

		// index count (how much to draw)
		int m_index_count;

		// mode to draw in
		GLenum m_mode = 0;
		bool m_wireframe = false;

		void draw();
		void destroy();
	};

	struct vertex_data {
		vec3 pos;
		vec3 norm;
		vec2 uv;
		vertex_data(vec3 p = {}, vec3 n = {}, vec2 t = {})
			: pos(p), norm(n), uv(t){ }
		vertex_data(vec3 p, vec2 t)
			: pos(p), norm(0, 0, 1), uv(t) { }
	};

	class mesh_data {
		public:
			std::vector<vertex_data> m_vertices;
			std::vector<unsigned int> m_indices;
			GLenum m_mode;
			bool m_wireframe;

			mesh_data(
				const std::vector<vertex_data> &vertices = {},
				const std::vector<unsigned int> &indices = {},
				GLenum mode = GL_TRIANGLES,
				bool wireframe = false
			);

			mesh upload(mesh m = {});
	};

}

