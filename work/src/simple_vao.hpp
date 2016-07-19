
#pragma once

#include <vector>

#include "cgra_math.hpp"
#include "opengl.hpp"

namespace cgra {

	class SimpleVAO {
		private:
			std::vector<float> m_positions;
			std::vector<float> m_normals;
			std::vector<float> m_uvs;

			vec3 m_currentNormal = vec3(0,0,1);
			vec2 m_currentUV = vec2(0,0);

			GLuint m_vao = 0;
			GLuint m_vbo_pos = 0;
			GLuint m_vbo_norm = 0;
			GLuint m_vbo_uv = 0;

			bool m_begin = false;
			GLenum m_mode = 0;

			GLuint m_vert_count = 0;

		public:
			SimpleVAO();
			~SimpleVAO();

			void begin(GLenum mode);
			void end();

			void draw();

			void set_normal(GLfloat x, GLfloat y, GLfloat z);
			void set_normal(GLfloat *v);
			void set_normal(vec3 v);

			void set_texcoord(GLfloat u, GLfloat v);
			void set_texcoord(GLfloat *v);
			void set_texcoord(vec2 v);

			void add_vertex(GLfloat x, GLfloat y, GLfloat z);
			void add_vertex(GLfloat *v);
			void add_vertex(vec3 v);

	};

}

