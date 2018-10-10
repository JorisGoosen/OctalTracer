#ifndef CUBEINTERSECT_H
#define CUBEINTERSECT_H
#include "commonfunctions.h"

class OctalNode;

std::string operator ~(glm::vec3 printMe);
std::string operator ~(glm::vec4 printMe);
std::ostream& operator<<(std::ostream & out, glm::vec3 const & streamMe);
std::ostream& operator<<(std::ostream & out, glm::vec4 const & streamMe);

struct foton
{
	foton(glm::vec3	pos, float phi, float theta, glm::vec3	rgb) : pos(pos), phi(phi), theta(theta), rgb(rgb) {}
	glm::vec3	pos;
	float		phi,
				theta;
	glm::vec3	rgb;
};

foton * GetCubeIntersectColor(OctalNode * Cubes, glm::vec3 Ray, glm::vec3 TotalCubeBounds[2], glm::vec3 Origin, glm::vec4 Kleur = glm::vec4(1.0f));

#endif // CUBEINTERSECT_H
