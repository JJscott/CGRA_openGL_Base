//
//#pragma once
//
//// std
//#include <fstream>
//#include <string>
//#include <sstream>
//#include <vector>
//
//// project
//#include "cgra_mesh.hpp"
//
//
//namespace cgra {
//
//	void load_wavefront_mesh(const std::string &filename) {
//
//		struct wavefront_vertex {
//			unsigned int p, n, t;
//		};
//
//		// create reading buffers
//		std::vector<vec3> positions;
//		std::vector<vec3> normals;
//		std::vector<vec2> uvs;
//		std::vector<wavefront_vertex> wv_vertices;
//
//		// create mesh data
//		std::vector<cgra::vertex> vertices;
//		std::vector<unsigned int> indices;
//
//		// open file
//		std::ifstream objFile(filename);
//		if (!objFile.is_open()) {
//			std::cerr << "Error: could not open " << filename << std::endl;
//			throw std::runtime_error("Error: could not open file.");
//		}
//
//		// good() means that failbit, badbit and eofbit are all not set
//		while (objFile.good()) {
//
//			// Pull out line from file
//			std::string line;
//			std::getline(objFile, line);
//			std::istringstream objLine(line);
//
//			// Pull out mode from line
//			std::string mode;
//			objLine >> mode;
//
//			// Reading like this means whitespace at the start of the line is fine
//			// attempting to read from an empty string/line will set the failbit
//			if (!objLine.fail()) {
//
//
//				if (mode == "v") {
//					vec3 v;
//					objLine >> v.x >> v.y >> v.z;
//					positions.push_back(v);
//				}
//				else if (mode == "vn") {
//					vec3 vn;
//					objLine >> vn.x >> vn.y >> vn.z;
//					normals.push_back(vn);
//
//				}
//				else if (mode == "vt") {
//					vec2 vt;
//					objLine >> vt.x >> vt.y;
//					uvs.push_back(vt);
//
//				}
//				else if (mode == "f") {
//
//					std::vector<wavefront_vertex> wv_verts;
//					while (objLine.good()) {
//						wavefront_vertex v;
//
//						objLine >> v.p; // Scan in position index
//						if (objLine.peek() == '/') {	// Look ahead for a match
//							objLine.ignore(1);			// Ignore the '/' character
//
//							if (objLine.peek() != '/') {
//								objLine >> v.t; // Scan in uv (texture coord) index
//							}
//
//							if (objLine.peek() == '/') {
//								objLine.ignore(1);
//								objLine >> v.n; // Scan in normal index
//							}
//						}
//
//						wv_verts.push_back(v);
//					}
//
//					// IFF we have 3 verticies, construct a triangle
//					if (wv_verts.size() == 3) {
//						for (int i = 0; i < 3; ++i) {
//							indices.push_back(vertices.size());
//							vertices.emplace_back(
//								positions[wv_verts[i].p - 1],
//								normals[wv_verts[i].n - 1],
//								uvs[wv_verts[i].t - 1]
//							);
//						}
//					}
//				}
//			}
//		}
//
//		std::cout << "Reading OBJ file is DONE." << std::endl;
//		std::cout << positions.size() << " points" << std::endl;
//		std::cout << normals.size() << " normals" << std::endl;
//		std::cout << uvs.size() << " uv coords" << std::endl;
//		std::cout << indices.size()/3 << " faces" << std::endl;
//
//
//		// If we didn't have any normals, create them
//		if (normals.size() <= 1) {
//			// Create the normals as 3d vectors of 0
//			for (size_t i = 1; i < m_points.size(); i++) {
//				m_normals.push_back(vec3());
//			}
//
//			// Add the normal for every face to each vertex-normal
//			for (size_t i = 0; i < m_triangles.size(); i++) {
//				vertex &a = m_triangles[i].v[0];
//				vertex &b = m_triangles[i].v[1];
//				vertex &c = m_triangles[i].v[2];
//
//				a.n = a.p;
//				b.n = b.p;
//				c.n = c.p;
//
//				vec3 ab = m_points[b.p] - m_points[a.p];
//				vec3 ac = m_points[c.p] - m_points[a.p];
//
//				vec3 norm = cross(ab, ac);
//				if (length(norm) > 0) {
//					m_normals[a.n] += normalize(norm);
//					m_normals[b.n] += normalize(norm);
//					m_normals[c.n] += normalize(norm);
//				}
//			}
//
//			// Normalize the normals
//			for (size_t i = 1; i < m_points.size(); i++) {
//				m_normals[i] = normalize(m_normals[i]);
//			}
//		}
//
//
//		for (int i = 0; i < 3; ++i) {
//			indices.push_back(vertices.size());
//			vertices.emplace_back(
//				positions[wv_verts[i].p - 1],
//				normals[wv_verts[i].n - 1],
//				uvs[wv_verts[i].t - 1]
//			);
//		}
//	}
//}