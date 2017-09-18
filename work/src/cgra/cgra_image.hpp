
#pragma once

// std
#include <chrono>
#include <memory>
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

		// default template
		template <typename T>
		struct stb_image_type_converter {
			inline T read(float f) const { return T(f); }
			inline T read(unsigned char c) const { return T(c); }

			inline void write(T t, unsigned char &out) const { out = unsigned char(t); }
			inline void write(T t, float &out) const { out = float(t); }
		};

		template <>
		struct stb_image_type_converter<unsigned char> {
			inline unsigned char read(float f) const { return unsigned char(f * 255); }
			inline unsigned char read(unsigned char c) const { return c; }

			inline void write(unsigned char t, unsigned char &out) const { out = t; }
			inline void write(unsigned char t, float &out) const { out = t / 255.f; }
		};

		template <>
		struct stb_image_type_converter<float> {
			inline float read(float f) const { return f; }
			inline float read(unsigned char c) const { return c / 255.f; }

			inline void write(float t, unsigned char &out) const { out = unsigned char(clamp(t, 0.f, 1.f) * 255.f); }
			inline void write(float t, float &out) const { out = t; }
		};

		template <>
		struct stb_image_type_converter<double> {
			inline double read(float f) const { return f; }
			inline double read(unsigned char c) const { return c / 255.0; }

			inline void write(double t, unsigned char &out) const { out = unsigned char(clamp(t, 0.0, 1.0) * 255.0); }
			inline void write(double t, float &out) const { out = float(t); }
		};
	}

	template <typename T, size_t> class image;
	using image1 = image<unsigned char, 1>;
	using image2 = image<unsigned char, 2>;
	using image3 = image<unsigned char, 3>;
	using image4 = image<unsigned char, 4>;
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

		// helper methods
		static int wrap_one(int i, int s, GLenum mode) {
			using namespace cgra;
			if (mode == GL_CLAMP_TO_EDGE) {
				return clamp(i, s - 1, 0);
			} else {
				// repeat
				// mod (plus s for negative term)
				return i % s + (i < 0) ? s : 0;
			}
		}

	public:
		image() : m_size(-1, -1) { }

		image(int w, int h) : m_size(w, h), m_data(m_size.x*m_size.y*N, 0) { }

		image(const std::string &filename) : m_size(-1, -1) {
			// vertical flipping
			stbi_set_flip_vertically_on_load(true);
			int w, h, n;

			if (stbi_is_hdr(filename.c_str())) {
				// floating point formats
				float *img = stbi_loadf(filename.c_str(), &w, &h, &n, N);
				if (!img) {
					std::cerr << "Error: Failed to load image " << filename << " : " << stbi_failure_reason();
					throw std::runtime_error("Failed to load image " + filename);
				}
				detail::stb_image_type_converter<T> converter;
				m_data = std::vector<T>(w * h * N);
				for (size_t i = 0; i < w * h * N; i++)
					m_data[i] = converter.read(img[i]);
				m_size = basic_vec<int, 2>(w, h);
				stbi_image_free(img);

			} else {
				// byte (0-255) formats
				unsigned char *img = stbi_load(filename.c_str(), &w, &h, &n, N);
				if (!img) {
					std::cerr << "Error: Failed to load image " << filename << " : " << stbi_failure_reason();
					throw std::runtime_error("Failed to load image " + filename);
				}

				detail::stb_image_type_converter<T> converter;
				m_data = std::vector<T>(w * h * N);
				for (size_t i = 0; i < w * h * N; i++)
					m_data[i] = converter.read(img[i]);
				m_size = basic_vec<int, 2>(w, h);
				stbi_image_free(img);
			}
		}

		// use to get a float pointer to the data
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


		// retrieves the texture pixel given the coordinates and wrapping mode
		vector_t & texel(float x, float y) { return reinterpret_cast<vector_t &>(m_data[N*(floor(x) + floor(y)*m_size.x)]); }
		const vector_t & texel(float x, float y) const { return reinterpret_cast<const vector_t &>(m_data[N*(floor(x) + floor(y)*m_size.x)]); }

		vector_t & texel(const basic_vec<float, 2> &c) { this->texel(c.x, c.y); }
		const vector_t & texel(const basic_vec<float, 2> &c) const { this->texel(c.x, c.y); }


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


		GLuint upload_texture(GLenum format = GL_RGBA8, GLuint tex = 0) const {
			if (!tex) glGenTextures(1, &tex);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap.x);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap.y);
			glTexImage2D(
				GL_TEXTURE_2D, 0, format, m_size.x, m_size.y, 0,
				detail::gl_image_format<N>::value, detail::gl_type_format<T>::value, m_data.data()
			);
			glGenerateMipmap(GL_TEXTURE_2D);
			return tex;
		}

		// outputs the image to the given filepath and appends ".png"
		void write_png(const std::string &filename) {
			std::vector<unsigned char> char_data(m_size.x*m_size.y*N, 0);
			detail::stb_image_type_converter<T> converter;
			for (size_t i = 0; i < m_size.x*m_size.y*N; i++)
				converter.write(m_data[i], char_data[i]);
			std::ostringstream ss;
			ss << filename << ".png";
			if (stbi_write_png(ss.str().c_str(), m_size.x, m_size.y, N, char_data.data() + (m_size.y-1)*m_size.x*N, -m_size.x * N)) {
				std::cout << "Wrote image: " << ss.str() << std::endl;
			} else {
				std::cerr << "Failed to write image: " << ss.str() << std::endl;
			}
		}

		// outputs the image to the given filepath and appends ".hdr"
		void write_hdr(const std::string &filename) {
			size_t s = m_size.x*m_size.y*N;
			std::vector<float> float_data(s, 0);
			detail::stb_image_type_converter<T> converter;
			for (size_t y = 0; y < m_size.y; y++)
				for (size_t x = 0; x < m_size.x; x++)
					for (size_t n = 0; n < N; n++)
						converter.write(
							m_data[n + N * (x + m_size.x * (y))],
							float_data[n + N * (x + m_size.x * (m_size.y - y - 1))]
						);
			std::ostringstream ss;
			ss << filename << ".hdr";
			if (stbi_write_hdr(ss.str().c_str(), m_size.x, m_size.y, N, float_data.data())) {
				std::cout << "Wrote image: " << ss.str() << std::endl;
			}
			else {
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
			glReadPixels(0, 0, w, h, detail::gl_image_format<N>::value, detail::gl_type_format<T>::value, img.data());

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
