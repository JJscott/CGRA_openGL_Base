
// std
#include <stdexcept>

// project
#include "cgra_mesh.hpp"


using namespace std;


// helper function
// returns the correct number of primitives given vertex index count and draw mode
int primitive_count(int index_count, GLuint mode) {
	// Set primitive count
	int patch_size;
	switch (mode) {
	case GL_POINTS:
	case GL_LINE_LOOP:
		return index_count;
	case GL_LINES:
	case GL_LINES_ADJACENCY:
	case GL_LINE_STRIP:
	case GL_LINE_STRIP_ADJACENCY:
		return index_count - 1;
	case GL_TRIANGLES:
	case GL_TRIANGLES_ADJACENCY:
		return index_count / 3;
	case GL_TRIANGLE_FAN:
	case GL_TRIANGLE_STRIP:
	case GL_TRIANGLE_STRIP_ADJACENCY:
		return index_count - 2;
	case GL_PATCHES:
		glGetIntegerv(GL_PATCH_VERTICES, &patch_size);
		return index_count / patch_size;
	default:
		throw runtime_error("Error: GLenum 'mode' is invalid");
	}
}

namespace cgra {

	mesh::mesh(GLenum mode, const std::vector<vertex> &vertices, const std::vector<unsigned int> &indices)
		: m_mode(mode), m_vertices(vertices), m_indices(indices) { }

	// copy ctors
	mesh::mesh(const mesh &other)
		: m_vertices(other.m_vertices), m_indices(other.m_indices), m_mode(other.m_mode), m_wireframe(other.m_wireframe) { }

	mesh & mesh::operator=(const mesh &other) {
		m_vao = {};
		m_vbo = {};
		m_ibo = {};

		m_vertices = other.m_vertices;
		m_indices = other.m_indices;

		m_mode = other.m_mode;
		m_wireframe = other.m_wireframe;

		return *this;
	}

	void mesh::reupload() {

		// Create Vertex Array Object (VAO) that can hold information
		// about how the VBOs are set up

		// Create a VAO, uses glGenVertexArrays(1, &m_vao);
		if (!m_vao) m_vao = gl_object::gen_vertex_array();

		// Create a vertex attribute buffer in GPU memory, uses glGenBuffers(1, &m_vbo);
		if (!m_vbo) m_vbo = gl_object::gen_buffer();

		// Create an index buffer in GPU memory, uses glGenBuffers(1, &m_ibo);
		if (!m_ibo) m_ibo = gl_object::gen_buffer();

		// Compile the vertex data into a single vector
		size_t vertex_length = 3 + 3 + 2; // pos, norm, uv (8 floats)
		std::vector<float> vertex_data(m_vertices.size() * vertex_length);
		for (size_t i = 0; i < m_vertices.size(); ++i) {
			// positions
			vertex_data[(i*vertex_length) + 0] = m_vertices[i].pos[0];
			vertex_data[(i*vertex_length) + 1] = m_vertices[i].pos[1];
			vertex_data[(i*vertex_length) + 2] = m_vertices[i].pos[2];
			// normals
			vertex_data[(i*vertex_length) + 3] = m_vertices[i].norm[0];
			vertex_data[(i*vertex_length) + 4] = m_vertices[i].norm[1];
			vertex_data[(i*vertex_length) + 5] = m_vertices[i].norm[2];
			// uvs
			vertex_data[(i*vertex_length) + 6] = m_vertices[i].uv[0];
			vertex_data[(i*vertex_length) + 7] = m_vertices[i].uv[1];
		}



		// VAO
		//
		glBindVertexArray(m_vao);

		

		// VBO (single buffer, interleaved)
		//
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		// Upload ALL the data giving it the size (in bytes) and a pointer to the data
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_data.size(), &vertex_data[0], GL_STATIC_DRAW);

		// This buffer will use location=0 when we use our VAO
		glEnableVertexAttribArray(0);
		// Tell opengl how to treat data in location=0
		// the data is treated in lots of 3 (3 floats = vec3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_length, (void*)(0));

		// Do the same thing for Normals but bind it to location=1
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_length, (void*)(sizeof(float) * 3));

		// Do the same thing for UVs but bind it to location=2
		// Also, we are setting up an array for lots of 2 floats (vec2) instead of 3 floats (vec3)
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_length, (void*)(sizeof(float) * 6));



		// IBO
		//
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);



		// Recalcuate number of primitives based on index
		m_primitive_count = primitive_count(m_indices.size(), m_mode);

		// Clean up by binding 0, good practice
		// the GL_ELEMENT_ARRAY_BUFFER binding sticks to the VAO so we shouldn't unbind it
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


	void mesh::draw() {

		if (!m_vao) reupload();

		// Set wireframe or fill polygon mode
		glPolygonMode(GL_FRONT_AND_BACK, (m_wireframe) ? GL_LINE : GL_FILL);
		// Bind our VAO which sets up all our buffers and data for us
		glBindVertexArray(m_vao);
		// Tell opengl to draw our VAO using the draw mode and how many verticies to render
		glDrawElements(m_mode, m_indices.size(), GL_UNSIGNED_INT, 0); // with indices
	}
}