
#include <stdexcept>

#include "simple_vao.hpp"


using namespace std;


namespace cgra {


	SimpleVAO::SimpleVAO() { }


	SimpleVAO::~SimpleVAO() {
		if (m_vao != 0) {
			glDeleteBuffers(1, &m_vbo_pos);
			glDeleteBuffers(1, &m_vbo_norm);
			glDeleteBuffers(1, &m_vbo_uv);
			glDeleteVertexArrays(1, &m_vao);
		}
	}


	void SimpleVAO::begin(GLenum mode) {
		if (m_begin) throw runtime_error("begin() can not be called twice");
		if (m_vao != 0) throw runtime_error("begin() can not be called after end()");

		m_mode = mode;
		m_begin = true;
	}


	void SimpleVAO::end() {
		if (m_vao == 0) {
			if (!m_begin) throw runtime_error("begin() must be called before end()");
			// Create Vertex Array Object (VAO) that can hold information
			// about how the VBOs should be set up
			glGenVertexArrays(1, &m_vao);
			// Tell opengl that we are going to work with this VAO now
			glBindVertexArray(m_vao);


			// Create a buffer in GPU memory, its ID is put into m_vbo_pos
			glGenBuffers(1, &m_vbo_pos);
			// Tell openGL that we are going to work on our buffer now
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_pos);
			// This buffer will use location=0 when we use our VAO
			glEnableVertexAttribArray(0);
			// Pass through ALL the data giving it the size (in bytes)
			// and a pointer to the data (in this case, the address of the first element in the vector)
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_positions.size(), &m_positions[0], GL_STATIC_DRAW);
			// Tell opengl how to treat data in location=0
			// the data is treated in lots of 3 (3 floats = vec3)
			// the other arguments are standard
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);


			// Do the same thing for Normals but bind it to location=1
			glGenBuffers(1, &m_vbo_norm);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_norm);
			glEnableVertexAttribArray(1);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_normals.size(), &m_normals[0], GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);


			// Do the same thing for UVs but bind it to location=2
			glGenBuffers(1, &m_vbo_uv);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_uv);
			glEnableVertexAttribArray(2);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_uvs.size(), &m_uvs[0], GL_STATIC_DRAW);
			// Also, we are setting up an array for lots of 2 floats (vec2) instead of 3 floats (vec3)
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);


			// Clean up by binding 0
			// not nessesary, but good practice
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);


			// Set primitive count
			int patch_size;
			switch (m_mode) {
			case GL_POINTS:
			case GL_LINE_LOOP:
				m_primitive_count = m_positions.size(); break;
			case GL_LINES:
			case GL_LINES_ADJACENCY:
			case GL_LINE_STRIP:
			case GL_LINE_STRIP_ADJACENCY:
				m_primitive_count = m_positions.size() - 1; break;
			case GL_TRIANGLES:
			case GL_TRIANGLES_ADJACENCY:
				m_primitive_count = m_positions.size() / 3; break;
			case GL_TRIANGLE_FAN:
			case GL_TRIANGLE_STRIP:
			case GL_TRIANGLE_STRIP_ADJACENCY:
				m_primitive_count = m_positions.size() - 2; break;
			case GL_PATCHES:
				glGetIntegerv(GL_PATCH_VERTICES, &patch_size);
				m_primitive_count = m_positions.size() / patch_size; break;
			default:
				throw runtime_error("GLenum 'mode' is invalid");
			}

			// Clean up the data
			vector<float>().swap(m_positions);
			vector<float>().swap(m_normals);
			vector<float>().swap(m_uvs);

		}
		else {
			throw runtime_error("end() can not be called twice");
		}
	}


	void SimpleVAO::draw() {
		if (m_vao != 0) {
			// Bind our VAO which sets up all our buffers and data for us
			glBindVertexArray(m_vao);
			// Tell opengl to draw our VAO using the draw mode and how many verticies to render
			glDrawArrays(m_mode, 0, m_primitive_count);
		} else {
			throw runtime_error("Can not draw uninitialized VAO");
		}
	}


	void SimpleVAO::set_normal(float x, float y, float z) {
		m_currentNormal = basic_vec<float, 3>(x, y, z);
	}

	void SimpleVAO::set_normal(float *v) {
		m_currentNormal = basic_vec<float, 3>(v[0], v[1], v[2]);
	}

	void SimpleVAO::set_normal(basic_vec<float, 3> v) {
		m_currentNormal = v;
	}



	void SimpleVAO::set_texcoord(float u, float v) {
		m_currentUV = basic_vec<float, 2>(u, v);
	}

	void SimpleVAO::set_texcoord(float *v) {
		m_currentUV = basic_vec<float, 2>(v[0], v[1]);
	}

	void SimpleVAO::set_texcoord(basic_vec<float, 2> v) {
		m_currentUV = v;
	}



	void SimpleVAO::add_vertex(float x, float y, float z) {
		if (!m_begin) throw runtime_error("Can not add vertex before calling begin()");
		if (m_vao != 0) throw runtime_error("Can not add vertex after calling end()");
		m_positions.push_back(x);
		m_positions.push_back(y);
		m_positions.push_back(z);

		m_normals.push_back(m_currentNormal.x);
		m_normals.push_back(m_currentNormal.y);
		m_normals.push_back(m_currentNormal.z);

		m_uvs.push_back(m_currentUV.x);
		m_uvs.push_back(m_currentUV.y);
	}

	void SimpleVAO::add_vertex(float *v) {
		add_vertex(v[0], v[1], v[2]);
	}

	void SimpleVAO::add_vertex(basic_vec<float, 3> v) {
		add_vertex(v.x, v.y, v.z);
	}
}