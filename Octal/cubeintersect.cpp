#include "cubeintersect.h"
#include "octalnode.h"
#include <iostream>
#include <sstream>

using namespace glm;

std::string operator ~(glm::vec3 printMe)
{
	std::stringstream out;
	out << "(" << printMe.x << ", " << printMe.y << ", " << printMe.z << ")";
	return out.str();
}

std::string operator ~(glm::vec4 printMe)
{
	std::stringstream out;
	out << "(" << printMe.x << ", " << printMe.y << ", " << printMe.z << ", " << printMe.w << ")";
	return out.str();
}

std::ostream& operator<<(std::ostream & out, glm::vec3 const & streamMe)
{
	out << ~streamMe;
	return out;
}

std::ostream& operator<<(std::ostream & out, glm::vec4 const & streamMe)
{
	out << ~streamMe;
	return out;
}

// http://chiranjivi.tripod.com/octrav.html
foton * GetCubeIntersectColor(OctalNode * Cubes, vec3 Ray, vec3 TotalCubeBounds[2], vec3 Origin, vec4 Kleur)
{
	vec4		NieuwKleur		= Kleur;// glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); //randvec4(0.0f, 1.0f);//			= vec3(1.0f);
	const uvec3	AssenMult		= uvec3(1, 2, 4);
	const vec3	InvRay			= vec3(1.0f) / Ray;
	const ivec3 Sign			= ivec3(int(Ray.x < 0), int(Ray.y < 0), int(Ray.z < 0));
	const vec3	CubeMin			= vec3(TotalCubeBounds[  Sign.x].x, TotalCubeBounds[  Sign.y].y, TotalCubeBounds[  Sign.z].z),
				CubeMax			= vec3(TotalCubeBounds[1-Sign.x].x, TotalCubeBounds[1-Sign.y].y, TotalCubeBounds[1-Sign.z].z),
				original_tmin	= (CubeMin - Origin) * InvRay,
				original_tmax	= (CubeMax - Origin) * InvRay;
	int			Diepte			= 0;
	float		minimumMaxMin	= 0.0f,
				minmax,
				maxmin;
	vec3		tmin			= original_tmin,
				tmax			= original_tmax,
				tmid;
	uint		SubIndex;
	OctalNode	*NodeIndex		= Cubes,
				*PrevNodeIndex	= NULL;

	/*std::cout <<	"Origin:  " << Origin		<< std::endl <<
					"Ray:     " << Ray		<< std::endl <<
					"Sign:    " << Sign		<< std::endl <<
					"CubeMin: " << CubeMin	<< std::endl <<
					"CubeMax: " << CubeMax	<< std::endl <<
					"tmin:    " << tmin		<< std::endl <<
					"tmax:    " << tmax		<< std::endl;*/

	while(Diepte >= 0)
	{
		maxmin = max(minimumMaxMin, max(max(tmin.x, tmin.y), tmin.z));
		minmax = min(min(tmax.x, tmax.y), tmax.z);

		//std::cout << "Entering Diepte " << Diepte << " maxmin = " << maxmin << " minmax " << minmax << std::endl;

		if(maxmin >= minmax || minmax < 0 || maxmin < 0)
			return NULL;

		tmid = (tmin + tmax) * 0.5f;

		if(NodeIndex != NULL)
		{
			Diepte++;
			SubIndex = 0;

			for(int as=0; as<3; as++)
				SubIndex += AssenMult[as] * uint(int(Sign[as] == int(tmid[as] > maxmin)));

			PrevNodeIndex	= NodeIndex;
			NodeIndex		= PrevNodeIndex->Sub[SubIndex];

			for(int i=0; i<3; i++)
				if(tmid[i] > maxmin)	tmax[i] = tmid[i];
				else					tmin[i] = tmid[i];
		}
		else
		{
			//return vec4(0.0f, 0.0f, 1.0f, 1.0f);
			vec4 GevondenNodeKleur =  PrevNodeIndex->Kleur;

			if(GevondenNodeKleur.a > 0.8f)
			{

				glm::vec3	pos = CubeMin + ( tmid * Ray ),
							normal;

				for(int i=0; i<3; i++)
					normal[i] = maxmin == tmin[i] ? (Sign[i] < 0 ? 1.0f : -1.0f) : 0.0f;

				normal = glm::normalize(normal);

				if(true) //randfloat(0.0f, 1.0f) > 0.1f)
				{
					PrevNodeIndex->Kleur += glm::abs(glm::dot(normal, Ray)) * 1.0f * Kleur;//glm::min(glm::vec4(1.0f), PrevNodeIndex->Kleur + (0.1f * Kleur));

					//lets return a position!
					glm::vec3	rgb = glm::vec3(1.0f, 1.0f, 1.0f),
								dir = Ray * -1.0f;
					float		phi = std::atan2(dir.y, dir.x),
								the	= std::atan2(dir.z, dir.x);

					return new foton(pos, phi, the, rgb);
				}
				else
				{
					return GetCubeIntersectColor(Cubes, glm::reflect(Ray, normal), TotalCubeBounds, pos, NieuwKleur);
				}

			}
			else
			{

				static int retryChances = 1000000;

				if(retryChances-- <= 0)
				{
					//std::cout << "foton got lost in loopspace" << std::endl;
					return NULL;
				}
				//else if(retryChances % 10 == 0)
				//	std::cout << "foton travelling through loopspace! " << retryChances / 10 << "% left!" << std::endl;

				Diepte			= 0;
				NodeIndex		= Cubes;
				PrevNodeIndex	= NULL;
				minimumMaxMin	= minmax + 0.00000000000000000001f;
				tmin			= original_tmin;
				tmax			= original_tmax;

			}
		}
	}

	return NULL;
}
