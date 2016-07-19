
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

		public:
			SimpleVAO();
			~SimpleVAO();

			void begin(GLenum mode);
			void end();

			void draw();

			void normal3f(GLfloat x, GLfloat y, GLfloat z);
			void normal3fv(GLfloat *v);
			void normal3f(vec3 v);

			void texCoord2f(GLfloat u, GLfloat v);
			void texCoord2fv(GLfloat *v);
			void texCoord2f(vec2 v);

			void vertex3f(GLfloat x, GLfloat y, GLfloat z);
			void vertex3fv(GLfloat *v);
			void vertex3f(vec3 v);

	};

}

