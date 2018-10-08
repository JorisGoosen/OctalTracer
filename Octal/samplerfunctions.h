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

	float pieMaker = (1.0f / dim) * 3.142f;

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

			hoogteKaart[x][y] = calcStapel * 0.5f;
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

glm::vec4 perliner(glm::vec3 c)
{
	float alpha = perlin->GetIniqoQuilesNoise(0.5f * c * glm::vec3(0.4f, 0.4f, 0.2f));

	float hoogte = powf(0.5f + (c.y * 0.5f), 3.0f);
	alpha *= hoogte;
	//return glm::vec4(randvec3(0.0f, 1.0f), alpha > 0.5f ? 0.8f : 0.0f);
	return glm::vec4(glm::vec3(1.0f), alpha > 0.5f ? 0.5f : 0.0f);
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


glm::vec4 simpleSampler(glm::vec3 coord)
{
	float innerDist = glm::length(coord) - 0.75f;

	float alpha  =  glm::clamp(-innerDist, 0.0f, 0.05f) * 20.0f;

	if(alpha < 0.5f)
		alpha = 0.0f;
	else
		alpha = 1.0f;

	//if(outerDist > 0 || innerDist < 0)
		return glm::vec4((coord * 0.5f) + 0.5f, alpha);
}

glm::vec4 simpleSamplerInverted(glm::vec3 coord)
{
	if(glm::length(coord) > 1.1f)
		return glm::vec4(randcolor(), 1.0f);
	return glm::vec4(0.0f);
}


glm::vec4 doubleWhammy(glm::vec3 coord)
{
	float outerDist = glm::length(coord) - 1.15f;
	float innerDist = glm::length(coord) - 0.75f;

	float alpha  =  glm::clamp(outerDist > 0 ? outerDist : -innerDist, 0.0f, 0.05f) * 20.0f;

	//if(outerDist > 0 || innerDist < 0)
		return glm::vec4((coord * 0.5f) + 0.5f, alpha);
	//return glm::vec4(0.0f);
}

glm::vec4 perlinNoise(glm::vec3 coord)
{
	//if( > 0.5)
		//return glm::vec4(, 1.0f);
	float noiseVal0 = 0.75 * Perlin::thePerlin()->GetIniqoQuilesNoise(coord * 0.025f);
	float noiseVal1 = Perlin::thePerlin()->GetIniqoQuilesNoise(glm::vec3(0.5f, 2.1f, 5.321f) + (coord * 0.1f));
	float noiseMax0 = glm::max(noiseVal0, noiseVal1);
	if(noiseMax0 < 0.5f) noiseMax0 = 0.0f;

	return glm::vec4((coord * 0.5f) + 0.5f, glm::clamp(noiseMax0, 0.0f, 1.0f));
}



glm::vec4 detailPerlinNoise(glm::vec3 coord)
{
	static glm::vec3 randOffset = randvec3(-1.0f, 1.0f);
	float noiseVal0 = Perlin::thePerlin()->GetIniqoQuilesNoise(randOffset + (coord * 0.1f));


	return glm::vec4((coord * 0.5f) + 0.5f, glm::clamp(glm::pow(noiseVal0, 2.0f), 0.0f, 1.0f));
}

glm::vec4 combine(glm::vec3 coord)
{
	glm ::vec4 a = simpleSampler(coord);
	glm ::vec4 b = detailPerlinNoise(coord);

	glm::vec3 rgb = glm::vec3(1.0f);
	float alpha = glm::min(a.a, b.a);
	return glm::vec4(glm::vec3(perlinNoise(coord.xyz).a, perlinNoise(coord.zxy).a, perlinNoise(coord.zyx).a) , alpha > 0.5f ? 1.0f : 0.0f);
}

glm::vec3 kleurSampler_0(glm::vec3 c)
{

	c *= pi * 2;// * 3;

	return glm::vec3( unicos(c.x), unicos(c.y), unisin(c.z));
}

glm::vec3 kleurPerlin_0(glm::vec3 c)
{
	static const glm::vec3		randOffset0 = randvec3(0.0f, 1.0f),
								randOffset1 = randvec3(0.0f, 1.0f),
								randOffset2 = randvec3(0.0f, 1.0f),
								mult( 0.02f, 1.0f, 3.0f );
	//c *= mult;

	glm::vec3	c0 = randOffset0 + ( c * mult.x ),
				c1 = randOffset1 + ( c * mult.y ),
				c2 = randOffset2 + ( c * mult.z );

	glm::vec3 col = glm::vec3(
				Perlin::thePerlin()->GetIniqoQuilesNoise(c0),
				Perlin::thePerlin()->GetIniqoQuilesNoise(c1),
				Perlin::thePerlin()->GetIniqoQuilesNoise(c2));

	return glm::vec3(std::max(col.r - (col.g + col.b), 0.0f), std::max(col.g - col.b, 0.0f), col.b);
}

glm::vec3 kleurGrijsPerlin_0(glm::vec3 c)
{
	static const glm::vec3		randOffset0 = randvec3(0.0f, 1.0f),
								randOffset1 = randvec3(0.0f, 1.0f),
								randOffset2 = randvec3(0.0f, 1.0f),
								mult( 4.0f, 3.00f, 2.0f );
	//c *= mult;

	glm::vec3	c0 = randOffset0 + ( c * mult.x ),
				c1 = randOffset1 + ( c * mult.y ),
				c2 = randOffset2 + ( c * mult.z );

	glm::vec3 col = glm::vec3(
				Perlin::thePerlin()->GetIniqoQuilesNoise(c0),
				Perlin::thePerlin()->GetIniqoQuilesNoise(c1),
				Perlin::thePerlin()->GetIniqoQuilesNoise(c2));

	float gray = std::max(col.x, std::max(col.y, col.z));

	return glm::vec3(0.35f + (0.1f * gray));
}

float hoogte(float x, float y)
{
	float extraZ = cosf(x * pi) * cosf(y * pi);

	x *= pi * 3;
	y *= pi * 3;

	float z = sinf(x) * cosf(y);

	z *= 0.5f;
	z += 0.5f * extraZ;

	return z * 0.5f;
}



#endif // SAMPLERFUNCTIONS_H
