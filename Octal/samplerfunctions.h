#ifndef SAMPLERFUNCTIONS_H
#define SAMPLERFUNCTIONS_H

#include "commonfunctions.h"
#include "Perlin.h"


glm::vec4 sineSampler(glm::vec3 coord)
{

	float alpha = std::max(sinf(coord.z * 3.1428f * 2), cosf(coord.x  * 3.1428f * 3) * cosf(coord.y * 3.1428f * 5));
		return glm::vec4((coord * 0.5f) + 0.5f, alpha > 0.5 ? 1.0f : 0.0f);

}

static Perlin * perlin = NULL;

void registerPerlinNoise(Perlin * p) { perlin = p; }


std::vector<std::vector<float>> hoogteVeldMaker(int dim)
{
	std::vector<std::vector<float>> hoogteKaart;

	float pieMaker = (1.0f / dim) * 3.1428f;

	hoogteKaart.resize(dim);

	for(int x=0; x<dim; x++)
	{
		hoogteKaart[x].resize(dim);

		for(int y=0; y<dim; y++)
		{
			float calcStapel = 0;

			float X = ((float)x) / ((float)dim);
			float Y = ((float)y) / ((float)dim);

			for(float z=0; z<1.0f; z+= 1.0f/dim)
				if(perlin->GetIniqoQuilesNoise(glm::vec3(X, Y, z) * 0.2f) > 0.5f)
					calcStapel += 1.0f/dim;
				else if
					(perlin->GetIniqoQuilesNoise(glm::vec3(X, Y, z) * 0.05f) > 0.5f)
					calcStapel += 0.25f/dim;

			hoogteKaart[x][y] = calcStapel * 0.7f;
		}
	}

	return hoogteKaart;

}

glm::vec3 bergKleur(glm::vec3 c)
{
	glm::vec3 colSneeuw(1.0f, 1.0f, 1.0f), colGrijs(0.25f, 0.25f, 0.25f), colGrijsGras(0.15f, 0.15f, 0.15f), colGras(0.05f, 0.4f, 0.0f);

	float grasMax = 0.1, sneeuwMin = 0.8f, grijsMin = 0.35;

	glm::vec3 nc = (c * 0.5f) + glm::vec3(0.5f);

	float hoogte = nc.y;

	if(hoogte < grasMax)
		return colGras;
	else if(hoogte >= grijsMin && hoogte <= sneeuwMin)
		return glm::mix(colGrijsGras, colGrijs, (hoogte - grijsMin) / (sneeuwMin - grijsMin));
	else if(hoogte > sneeuwMin)
		return glm::mix(colGrijs, colSneeuw, (hoogte - sneeuwMin) / (1.0f - sneeuwMin));
	else
		return glm::mix(colGras, colGrijsGras, (hoogte - grasMax) / (grijsMin - grasMax));
}

glm::vec4 berg(glm::vec3 c)
{
	const int dim = 100;
	static auto hoogtes = hoogteVeldMaker(dim);

	glm::vec3 nc = (c * 0.5f) + glm::vec3(0.5f);

	int x = nc.x * dim;
	int z = nc.z * dim;

	float alpha = nc.y < hoogtes[x][z]  ? 1.0f : 0.0f;

	return glm::vec4(bergKleur(c), alpha);

}

glm::vec3 colorMod(glm::vec3 c)
{
	glm::vec3 colA(0.0f, 0.0f, 1.0f), colB(1.0f, 1.0f, 0.0f);

	return glm::mix(colB, colA, powf(std::min(1.0f, glm::length(c)), 2.0f));
}

glm::vec4 bubbelbol(glm::vec3 c)
{
	glm::vec3 nc = (c * 0.5f) + glm::vec3(0.5f);

	float xz = atan2f(c.x, c.z);
	float xy = atan2f(c.x, c.y);
	float yz = atan2f(c.y, c.z);

	float l = length(c);

	float rad = 0.5f * sinf((xz * 3.142f * 6) + (xy * 3.142f * 3.0f) - (yz * 3.142f * 6.0f)) ;//- cosf(xy * 3.142f * 2);// * cosf(yz * 3.142f * 2);
	rad += 0.5f;

	return glm::vec4(colorMod(c), rad > l ? 1.0f : 0.0f);

}

#endif // SAMPLERFUNCTIONS_H
