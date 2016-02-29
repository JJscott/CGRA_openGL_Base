
#pragma once

#include "cgra_math.hpp"
#include "opengl.hpp"

#include <string>
#include <sstream>
#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

namespace cgra {

	struct image {
		int w, h;
		std::vector<unsigned char> data;

		image(int w_, int h_) : w(w_), h(h_), data(w*h*4, 0) {}

		image(const std::string &filepath) {
			unsigned char *stbi_data = stbi_load(filepath.c_str(), &w, &h, nullptr, 4);
			if (stbi_data == NULL)
				throw std::runtime_error("failed to load image " + filepath);
			data.assign(stbi_data, stbi_data+(w*h*4));
			stbi_image_free(stbi_data);
		}


		// copy functions
		image(const image &) = default;
		image & operator=(const image &) = default;
		image(image &&) = default;
		image & operator=(image &&) = default;


		// returns a vector with RGBA values from the given cell
		ivec4 getPixel(int x, int y) {
			assert(x >=0 && y >=0 && x < w && y < h);
			int idx = (x+(y*w)) * 4;
			return ivec4(data[idx], data[idx+1], data[idx+2], data[idx+3]);
		}


		// Use to get the appropriate GL format for data
		GLenum getGLFormat() const { return GL_RGBA; }


		// Use to get a GL friendly pointer to the data
		unsigned char * dataPointer() { return &data[0]; }
		const unsigned char * dataPointer() const { return &data[0]; }


		// outputs the image to the given filepath and appends ".png"
		image save(const std::string &filepath) {
			std::ostringstream ss;
			ss << filepath << ".png";
			stbi_write_png(ss.str().c_str(), w, h, 4, &data[0], w*4);
		}


		// returns an image resized to the given parameters
		image resize(int width, int height) const {
			assert(width >=1 && height >= 1);
			image img(width, height); 
			stbir_resize_uint8(&data[0], w, h, 0,
				&img.data[0], width, height, 0, 4);

			return img;
		}


		// returns a image with the subsection 
		image subsection(int xoffset, int yoffset, int width, int height) {
			image r(width, height);

			for (int y = 0; y < height; y++) {
				if ((y+yoffset) >= h) continue; 
				for(int x = 0; x < width; x++) {
					if ((x+xoffset) >= w) continue; 
					for (int i = 0; i < 4; i++) {
						r.data[ (y*width*4) + (x*4) + i ] = 
							data[ ((y+yoffset)*w*4) + ((x+xoffset)*4) + i ];
					}
				}
			}
			return r;
		}
	};
}