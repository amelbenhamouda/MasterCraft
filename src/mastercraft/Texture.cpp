#include <cmath>
#include <vector>
#include <iostream>
#include "glimac/common.hpp"
#include "mastercraft/Texture.hpp"

namespace glimac {

	void Texture::bindAndDrawTexture(GLuint texture, int x, int y) {
	    glBindTexture(GL_TEXTURE_2D, texture);
	    glDrawArrays(GL_TRIANGLES, x, y);
	    glBindTexture(GL_TEXTURE_2D,0);
	}

	void Texture::firstBindTexture(std::unique_ptr<Image> &texLoad, GLuint* texture) {
	    glGenTextures(1, texture); // on génére l'elem
	    glBindTexture(GL_TEXTURE_2D, *texture); // on le bind avec GL_TEXTURE_2D
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texLoad->getWidth(), texLoad->getHeight(), 0, GL_RGBA, GL_FLOAT, texLoad->getPixels()); // obtenir la texture à partir d'une image

	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glBindTexture(GL_TEXTURE_2D, 0);
	}

	std::unique_ptr<Image> Texture::loadTexture(std::string texture){
	    std::unique_ptr<Image> texLoad = loadImage(texture); // chargement EarthMap avec loadimage de glimax dans image.cpp
	    return texLoad;
	}
}
