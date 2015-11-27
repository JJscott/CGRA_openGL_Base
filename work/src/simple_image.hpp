
#pragma once

#include "cgra_math.hpp"
#include "opengl.hpp"

#include <stb_image.h>

namespace cgra {

	struct image {
		int w, h, n;
		std::vector<unsigned char> data;

		image(int w_, int h_, int n_) : w(w_), h(h_), n(n_), data(w*h*n, 0) {}

		image(const std::string &filepath) {
			unsigned char *stbi_data = stbi_load(filepath.c_str(), &w, &h, &n, 0);
			if (stbi_data == NULL)
				throw std::runtime_error("failed to load image " + filepath);
			data.assign(stbi_data, stbi_data+(w*h*n));
			stbi_image_free(stbi_data);
		}

		image(const image &) = default;
		image & operator=(const image &) = default;
		image(image &&) = default;
		image & operator=(image &&) = default;


		// Use to get the appropriate GL format for data
		GLenum glFormat() const {
			switch (n) {
			case 1: return GL_R;
			case 2: return GL_RG;
			case 3: return GL_RGB;
			case 4: return GL_RGBA;
			default: return GL_RGBA; // TODO throw exception?
			}
		}

		// Use to get a GL friendly pointer to the data
		unsigned char * dataPointer() { return &data[0]; }
		const unsigned char * dataPointer() const { return &data[0]; }

		image subImage(int xoffset, int yoffset, int width, int height) {
			image r(width, height, n);

			for (int y = 0; y < height; y++) {
				if ((y+yoffset) >= h) continue; 
				for(int x = 0; x < width; x++) {
					if ((x+xoffset) >= w) continue; 
					for (int i = 0; i < n; i++) {
						r.data[ (y*width*n) + (x*n) + i ] = 
							data[ ((y+yoffset)*w*n) + ((x+xoffset)*n) + i ];
					}
				}
			}
			return r;
		}
	};
}