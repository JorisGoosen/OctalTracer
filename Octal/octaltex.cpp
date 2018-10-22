#include "octaltex.h"

size_t OctalTex::Diepte		= 3;
size_t OctalTex::Dim		= size_t(pow(2, Diepte));
size_t OctalTex::rgbSize	= Dim * Dim * Dim;

OctalTex::OctalTex()
{
	static size_t baseIndex = 1;
	myIndex = baseIndex++;

	rgbas.resize(rgbSize);

	for(glm::vec4 & rgba : rgbas)
		rgba = glm::vec4(0.0f);
}

size_t OctalTex::toIndex(glm::uvec3 p)
{
	unsigned int minval = Dim - 1u;

	for(size_t i=0; i<3; i++)
		p[i] = std::min(minval, std::max(0u, p[i]));

	size_t index = p.x + (Dim * p.y) + (Dim * Dim * p.z);

	return index;
}

void OctalTex::setPixel(glm::uvec3 p, glm::vec4 c)
{
	rgbas[toIndex(p)] = c;
}

glm::vec4 OctalTex::getPixel(glm::uvec3 p)
{
	return rgbas[toIndex(p)];
}
