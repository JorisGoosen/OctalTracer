#ifndef SAMPLERFUNCDEFS_H
#define SAMPLERFUNCDEFS_H

#include "commonfunctions.h"

typedef glm::vec4	(*samplerFunc)			(glm::vec3);
typedef float		(*heightSamplerFunc)	(float, float);
typedef glm::vec3	(*colorSamplerFunc)		(glm::vec3);

#endif // SAMPLERFUNCDEFS_H
