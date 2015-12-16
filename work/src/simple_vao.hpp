
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
			GLenum m_mode;

		public:
			SimpleVAO();
			~SimpleVAO();

			void glBegin(GLenum mode);
			void glEnd();

			void draw();

			void glNormal3f(GLfloat x, GLfloat y, GLfloat z);
			void glNormal3fv(GLfloat *v);
			void glNormal3f(vec3 v);

			void glTexCoord2f(GLfloat u, GLfloat v);
			void glTexCoord2fv(GLfloat *v);
			void glTexCoord2f(vec2 v);

			void glVertex3f(GLfloat x, GLfloat y, GLfloat z);
			void glVertex3fv(GLfloat *v);
			void glVertex3f(vec3 v);

	};

}

