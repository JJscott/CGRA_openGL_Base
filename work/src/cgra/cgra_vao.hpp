
#pragma once

#include <vector>

#include "cgra_math.hpp"
#include "../opengl.hpp"

namespace cgra {

	class SimpleVAO {
		private:
			std::vector<float> m_positions;
			std::vector<float> m_normals;
			std::vector<float> m_uvs;

			basic_vec<float, 3> m_currentNormal = basic_vec<float, 3>(0,0,1);
			basic_vec<float, 2> m_currentUV = basic_vec<float, 2>(0,0);

			GLuint m_vao = 0;
			GLuint m_vbo_pos = 0;
			GLuint m_vbo_norm = 0;
			GLuint m_vbo_uv = 0;

			bool m_begin = false;
			GLenum m_mode = 0;
			GLuint m_primitive_count = 0;

		public:
			SimpleVAO();
			~SimpleVAO();

			void begin(GLenum mode);
			void end();

			void draw();

			void set_normal(float x, float y, float z);
			void set_normal(float *v);
			void set_normal(basic_vec<float, 3> v);

			void set_texcoord(float u, float v);
			void set_texcoord(float *v);
			void set_texcoord(basic_vec<float, 2> v);

			void add_vertex(float x, float y, float z);
			void add_vertex(float *v);
			void add_vertex(basic_vec<float, 3> v);

	};

}

