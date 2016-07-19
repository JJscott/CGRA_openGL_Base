
#pragma once

#include <chrono>
#include <string>
#include <sstream>
#include <vector>

#include "cgra_math.hpp"
#include "opengl.hpp"

#include <string>
#include <sstream>
#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

namespace cgra {

	template <size_t N> struct gl_image_format { };
	template <> struct gl_image_format<1> : std::integral_constant<GLenum, GL_RED> { };
	template <> struct gl_image_format<2> : std::integral_constant<GLenum, GL_RG> { };
	template <> struct gl_image_format<3> : std::integral_constant<GLenum, GL_RGB> { };
	template <> struct gl_image_format<4> : std::integral_constant<GLenum, GL_RGBA> { };

	template <size_t N>
	class image {
	public:
		using vector_t = vec_by_size_t<float, N>;

	private:
		uvec2 m_size;
		std::vector<float> m_data;
		vector2<GLenum> m_wrap {GL_REPEAT, GL_REPEAT};

		static int wrap_one(int i, int s, GLenum mode) {
			using namespace cgra;
			if (mode == GL_CLAMP_TO_EDGE) {
				return max(min(i, s - 1), 0);
			} else {
				// repeat
				// hacky, needs mod not remainder
				return (i + 100 * s) % s;
			}
		}

	public:
		image() { }

		image(int w, int h) : m_size(w, h), m_data(m_size.x*m_size.y*N, 0) { }

		image(const std::string &filename) {
			stbi_set_flip_vertically_on_load(true);
			int w, h;
			if (stbi_is_hdr(filename.c_str())) {
				float *img = stbi_loadf(filename.c_str(), &w, &h, nullptr, 4);
				if (!img) {
					std::cerr << "Failed to load image " << filename << " : " << stbi_failure_reason();
					throw std::runtime_error("failed to load image " + filename);
				} else {
					m_data.assign(img, img + w * h * 4);
					m_size = uvec2(w, h);
					stbi_image_free(img);
				}
			} else {
				unsigned char *img = stbi_load(filename.c_str(), &w, &h, nullptr, 4);
				if (!img) {
					std::cerr << "Failed to load image " << filename << " : " << stbi_failure_reason();
					throw std::runtime_error("failed to load image " + filename);
				} else {
					m_data.assign(w * h * 4, 0.f);
					for (int i = 0; i < w * h * 4; i++) {
						m_data[i] = img[i] / 255.f;
					}
					m_size = uvec2(w, h);
					stbi_image_free(img);
				}
			}
		}


		// returns a vector with RGBA values from the given cell
		vector_t & texel(int x, int y) { return reinterpret_cast<vector_t &>(m_data[x + y*m_size.x]); }
		const vector_t & texel(int x, int y) const { return reinterpret_cast<const vector_t &>(m_data[x + y*m_size.x]); }

		vector_t & texel(const ivec2 &c) { return reinterpret_cast<vector_t &>(m_data[c.x + c.y*m_size.x]); }
		const vector_t & texel(const ivec2 &c) const { return reinterpret_cast<const vector_t &>(m_data[c.x + c.y*m_size.x]); }


		// bilinear filtering with wrapping
		// returns floating point of corrosponding components
		vector_t texture(const vec2 &tc) const {
			vec2 p0 = tc * m_size;
			vector_t r { 0 };
			for (int y = 0; y < 2; y++) {
				for (int x = 0; x < 2; x++) {
					ivec2 p1 = ivec2(floor(p0 - 0.5f)) + ivec2(x, y);
					vec2 d = 1.f - abs(p1 + 0.5f - p0);
					p1.x = wrap_one(p1.x, m_size.x, m_wrap.x);
					p1.y = wrap_one(p1.y, m_size.y, m_wrap.y);
					r += texel(p1) * d.x * d.y;
				}
			}
			return r;
		}


		GLuint make_texture(GLenum format = GL_RGBA8, GLuint tex = 0) const {
			if (!tex) glGenTextures(1, &tex);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap.x);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap.y);
			glTexImage2D(GL_TEXTURE_2D, 0, format, m_size.x, m_size.y, 0, gl_image_format<N>::value, GL_FLOAT, data());
			glGenerateMipmap(GL_TEXTURE_2D);
			return tex;
		}


		// Use to get a float pointer to the data
		float * data() { return m_data.data(); }
		const float * data() const { return m_data.data(); }


		// outputs the image to the given filepath and appends ".png"
		void save(const std::string &filepath) {
			std::vector<unsigned char> char_data(m_size.x*m_size.y*N, 0);
			for (size_t i = 0; i < m_size.x*m_size.y*N; i++)
				char_data[i] = 255 * m_data[i];
			std::ostringstream ss;
			ss << filepath << ".png";
			stbi_write_png(ss.str().c_str(), m_size.x, m_size.y, 4, char_data.data() + (m_size.y-1)*m_size.x*N, -m_size.x * 4);
		}

		static image screenshot(bool save) {
			using namespace std;
			int w, h;
			glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);

            image img(w, h);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            glReadPixels(0, 0, w, h, GL_RGBA, GL_FLOAT, img.data());

            if (save) {
	            ostringstream filename_ss;
	            filename_ss << "screenshot_" << (chrono::system_clock::now().time_since_epoch() / 1ms) << ".png";
	           	string filename = filename_ss.str();
	            if (stbi_write_png(filename.c_str(), w, h, 4, data.data() + (h - 1) * w * 4, -w * 4)) {
	                cout << "Wrote screenshot " << filename << endl;
	            } else {
	                cerr << "Failed to write screenshot " << filename << endl;
	            }
	        }

            return img;
		}


		// // returns an image resized to the given parameters
		// image resize(int width, int height) const {
		// 	assert(width >=1 && height >= 1);
		// 	image img(width, height); 
		// 	stbir_resize_uint8(&data[0], w, h, 0,
		// 		&img.data[0], width, height, 0, 4);

		// 	return img;
		// }


		// // returns a image with the subsection 
		// image subsection(int xoffset, int yoffset, int width, int height) {
		// 	image r(width, height);

		// 	for (int y = 0; y < height; y++) {
		// 		if ((y+yoffset) >= h) continue; 
		// 		for(int x = 0; x < width; x++) {
		// 			if ((x+xoffset) >= w) continue; 
		// 			for (int i = 0; i < 4; i++) {
		// 				r.data[ (y*width*4) + (x*4) + i ] = 
		// 					data[ ((y+yoffset)*w*4) + ((x+xoffset)*4) + i ];
		// 			}
		// 		}
		// 	}
		// 	return r;
		// }



	};
}