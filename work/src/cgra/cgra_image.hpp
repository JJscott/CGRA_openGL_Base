
#pragma once

// std
#include <chrono>
#include <string>
#include <sstream>
#include <vector>

// stb
#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

// project
#include "cgra_math.hpp"
#include <opengl.hpp>


namespace cgra {

	namespace detail {
		template <size_t N> struct gl_image_format { };
		template <> struct gl_image_format<1> : std::integral_constant<GLenum, GL_RED> { };
		template <> struct gl_image_format<2> : std::integral_constant<GLenum, GL_RG> { };
		template <> struct gl_image_format<3> : std::integral_constant<GLenum, GL_RGB> { };
		template <> struct gl_image_format<4> : std::integral_constant<GLenum, GL_RGBA> { };


		template <typename T> struct gl_type_format { };
		template <> struct gl_type_format<char> : std::integral_constant<GLenum, GL_BYTE> { };
		template <> struct gl_type_format<short> : std::integral_constant<GLenum, GL_SHORT> { };
		template <> struct gl_type_format<int> : std::integral_constant<GLenum, GL_INT> { };
		template <> struct gl_type_format<unsigned char> : std::integral_constant<GLenum, GL_UNSIGNED_BYTE> { };
		template <> struct gl_type_format<unsigned short> : std::integral_constant<GLenum, GL_UNSIGNED_SHORT> { };
		template <> struct gl_type_format<unsigned int> : std::integral_constant<GLenum, GL_UNSIGNED_INT> { };
		template <> struct gl_type_format<float> : std::integral_constant<GLenum, GL_FLOAT> { };


		template <typename T>
		struct stb_image_interpreter {
			inline T operator()(float f) const { return T{ f }; }
			inline T operator()(unsigned char c) const { return T{ c }; }
		};

		template <>
		struct stb_image_interpreter<float> {
			inline float operator()(float f) const { return f; }
			inline float operator()(unsigned char c) const { return c / 255.f; }
		};
	}

	template <typename T, size_t> class image;
	using image1f = image<float, 1>;
	using image2f = image<float, 2>;
	using image3f = image<float, 3>;
	using image4f = image<float, 4>;


	template <typename T, size_t N>
	class image {
	public:
		using vector_t = basic_vec<T, N>;

	private:
		basic_vec<int, 2> m_size;
		basic_vec<GLenum, 2> m_wrap {GL_REPEAT, GL_REPEAT};
		std::vector<T> m_data;

		// helper method
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
		image() : m_size(-1, -1) { }

		image(int w, int h) : m_size(w, h), m_data(m_size.x*m_size.y*N, 0) { }

		image(const std::string &filename) : m_size(-1, -1) {
			stbi_set_flip_vertically_on_load(true);
			int w, h;

			if (stbi_is_hdr(filename.c_str())) {
				float *img = stbi_loadf(filename.c_str(), &w, &h, nullptr, N);
				if (!img) {
					std::cerr << "Error: Failed to load image " << filename << " : " << stbi_failure_reason();
					throw std::runtime_error("Failed to load image " + filename);
				} else {
					detail::stb_image_interpreter<T> interpreter;
					m_data = std::vector<T>(w * h * N);
					for (size_t i = 0; i < w * h * N; i++)
						m_data[i] = interpreter(img[i]);
					m_size = basic_vec<int, 2>(w, h);
					stbi_image_free(img);
				}
			} else {
				unsigned char *img = stbi_load(filename.c_str(), &w, &h, nullptr, N);
				if (!img) {
					std::cerr << "Error: Failed to load image " << filename << " : " << stbi_failure_reason();
					throw std::runtime_error("Failed to load image " + filename);
				} else {
					detail::stb_image_interpreter<T> interpreter;
					m_data = std::vector<T>(w * h * N);
					for (size_t i = 0; i < w * h * N; i++)
						m_data[i] = interpreter(img[i]);
					m_size = basic_vec<int, 2>(w, h);
					stbi_image_free(img);
				}
			}
		}

		// Use to get a float pointer to the data
		T * data() { return m_data.data(); }
		const T * data() const { return m_data.data(); }

		// dimensions
		int width() const { return m_size.x; }
		int height() const { return m_size.y; }
		basic_vec<int, 2> size() const { return m_size; }

		// channels
		size_t channels() const { return N; }

		// wrapping strategy that accepts GL_CLAMP_TO_EDGE and GL_REPEAT
		// works only for texture(const basic_vec<float, 2> &)
		basic_vec<GLenum, 2> wrap() const { return m_wrap; }
		void wrap(basic_vec<GLenum, 2> w) { m_wrap = w; }


		// returns a vector with RGBA values from the given cell
		vector_t & texel(int x, int y) { return reinterpret_cast<vector_t &>(m_data[x + y*m_size.x]); }
		const vector_t & texel(int x, int y) const { return reinterpret_cast<const vector_t &>(m_data[x + y*m_size.x]); }

		vector_t & texel(const basic_vec<int, 2> &c) { return reinterpret_cast<vector_t &>(m_data[c.x + c.y*m_size.x]); }
		const vector_t & texel(const basic_vec<int, 2> &c) const { return reinterpret_cast<const vector_t &>(m_data[c.x + c.y*m_size.x]); }


		// bilinear filtering with wrapping
		// returns floating point of corrosponding components
		vector_t texture(const basic_vec<float, 2> &tc) const {
			basic_vec<float, 2> p0 = tc * m_size;
			vector_t r { 0 };
			for (int y = 0; y < 2; y++) {
				for (int x = 0; x < 2; x++) {
					basic_vec<int, 2> p1 = basic_vec<int, 2>(floor(p0 - 0.5f)) + basic_vec<int, 2>(x, y);
					basic_vec<float, 2> d = 1.f - abs(p1 + 0.5f - p0);
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
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				format,
				m_size.x,
				m_size.y,
				0,
				detail::gl_image_format<N>::value,
				detail::gl_type_format<T>::value,
				&m_data[0]
			);
			glGenerateMipmap(GL_TEXTURE_2D);
			return tex;
		}

		// outputs the image to the given filepath and appends ".png"
		// TODO make generic of jpg, hdr etc.
		void write_png(const std::string &filename) {
			std::vector<unsigned char> char_data(m_size.x*m_size.y*N, 0);
			for (size_t i = 0; i < m_size.x*m_size.y*N; i++)
				char_data[i] = 255 * m_data[i];
			std::ostringstream ss;
			ss << filename << ".png";
			if (stbi_write_png(ss.str().c_str(), m_size.x, m_size.y, N, char_data.data() + (m_size.y-1)*m_size.x*N, -m_size.x * N)) {
				std::cout << "Wrote image: " << ss.str() << std::endl;
			} else {
				std::cerr << "Failed to write image: " << ss.str() << std::endl;
			}
		}

		// creates an image from FB 0
		static image screenshot(bool write) {
			using namespace std;
			int w, h;
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);

			image img(w, h);
			glReadPixels(0, 0, w, h, detail::gl_image_format<N>::value, GL_FLOAT, img.data());

			if (write) {
				ostringstream filename_ss;
				filename_ss << "screenshot_" << (chrono::system_clock::now().time_since_epoch() / 1ms);
				string filename = filename_ss.str();
				img.write_png(filename);
			}

			return img;
		}
	};
}
