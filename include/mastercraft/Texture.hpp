#pragma once
#include <iostream>
#include <vector>
#include "../glimac/common.hpp"
#include "../glimac/Image.hpp"

namespace glimac {

	// Représente une sphère discrétisée centrée en (0, 0, 0) (dans son repère local)
	// Son axe vertical est (0, 1, 0) et ses axes transversaux sont (1, 0, 0) et (0, 0, 1)
	class Texture {
		public:
		    void bindAndDrawTexture(GLuint texture, int x, int y);
		    void firstBindTexture(std::unique_ptr<Image> &texLoad, GLuint* texture);
		    std::unique_ptr<Image> loadTexture(std::string texture);
	};

}
