
#pragma once

// std
#include <vector>

// project
#include "cgra_math.hpp"
#include <opengl.hpp>

namespace cgra {

	struct vertex {
		vec3 pos;
		vec3 norm;
		vec2 uv;
		vertex(vec3 p = {}, vec3 n = {}, vec2 t = {})
			: pos(p), norm(n), uv(t){ }
		vertex(vec3 p, vec2 t)
			: pos(p), norm(0, 0, 1), uv(t) { }
	};

	class mesh {
		private:
			gl_object m_vao;
			gl_object m_vbo;
			gl_object m_ibo;
			int m_primitive_count;

		public:
			std::vector<vertex> m_vertices;
			std::vector<unsigned int> m_indices;
			GLenum m_mode = 0;
			bool m_wireframe = false;

			mesh(
				GLenum mode = GL_TRIANGLES,
				const std::vector<vertex> &vertices = {},
				const std::vector<unsigned int> &indices = {}
			);

			~mesh() { } // not needed, but the copy ctors are (because of gl_object)

			// copy ctors
			mesh(const mesh &);
			mesh & operator=(const mesh &);

			// move ctors
			mesh(mesh &&other) = default;
			mesh & operator=(mesh &&other) = default;

			void reupload();

			void draw();
	};

}

