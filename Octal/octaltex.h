#ifndef OCTALTEX_H
#define OCTALTEX_H

#include "commonfunctions.h"

struct OctalTex
{
	OctalTex();

	void setPixel(glm::uvec3 p, glm::vec4 c);
	void setPixel(size_t x, size_t y, size_t z, glm::vec4 c) { setPixel(glm::uvec3(x, y, z), c); }

	glm::vec4 getPixel(glm::uvec3 p);
	glm::vec4 getPixel(size_t x, size_t y, size_t z, glm::vec4 c) { return getPixel(glm::uvec3(x, y, z)); }

	size_t toIndex(glm::uvec3 p);

	static size_t	Diepte,
					Dim,
					rgbSize;

	std::vector<glm::vec4>	rgbas;

	size_t			myIndex;
};

#endif // OCTALTEX_H
