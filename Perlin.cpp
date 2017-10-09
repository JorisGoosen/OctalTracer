#include "Perlin.h"

Perlin::Perlin(QOpenGLFunctions_4_5_Core *QTGL)
{
    GL = QTGL;
	Init();
}

void Perlin::Init()
{
	InitGradients();
	InitPermutationTable();
}

Perlin::~Perlin()
{
	if(RandomGradients != NULL)		delete RandomGradients;
	if(PermutationTable != NULL)	delete PermutationTable;

	RandomGradients		= NULL;
	PermutationTable	= NULL;
}

void Perlin::InitGradients()
{
	if(RandomGradients != NULL) delete RandomGradients;

    RandomGradients = new shaderstorage<glm::mediump_vec3>(PERLIN_NUM_GRADIENTS, GL);

	for(int i=0; i<PERLIN_NUM_GRADIENTS; i++)
	{
        glm::vec3 P = randvec3(-1.0f, 1.0f);
		
        while(glm::length(P) > 1.0 || glm::length(P) == 0.0f)
            P = randvec3(-1.0f, 1.0f);

        (*RandomGradients)[i] =  glm::normalize(P);
	}

	RandomGradients->SchrijfWeg();
}

void Perlin::InitPermutationTable()
{
	if(PermutationTable != NULL) delete PermutationTable;

    PermutationTable = new shaderstorage<int>(PERLIN_NUM_GRADIENTS * 2, GL);

	for(int i=0; i<PERLIN_NUM_GRADIENTS; i++)
		(*PermutationTable)[i] = i;

	for(int i=0; i<PERLIN_NUM_GRADIENTS; i++)
	{
		int j = rand()%PERLIN_NUM_GRADIENTS;
		int Tmp = (*PermutationTable)[i];
		(*PermutationTable)[i] = (*PermutationTable)[j];
		(*PermutationTable)[j] = Tmp;
	}

	for(int i=0; i<PERLIN_NUM_GRADIENTS; i++)
		(*PermutationTable)[i + PERLIN_NUM_GRADIENTS] = (*PermutationTable)[i];
	
	PermutationTable->SchrijfWeg();
}


glm::mediump_vec3 Perlin::GetGradient(glm::ivec3 loc)
{
    int ijk[3];
    for(int i=0; i<3; i++)
        ijk[i] = ((int)loc[i]) % PERLIN_NUM_GRADIENTS;

    int DezeGradient = (*PermutationTable)[(*PermutationTable)[(*PermutationTable)[ijk[0]] + ijk[1]] + ijk[2]];

    return (*RandomGradients)[DezeGradient];
}


float Perlin::GetIniqoQuilesNoise(glm::vec3 x)
{
	x *= 16.0f;
    // grid
    glm::vec3 p = glm::floor(x), w = glm::fract(x);

    // quintic interpolant
    glm::vec3 u = w * w * w * (w * (w * glm::vec3(6.0f - 15.0f))+glm::vec3(10.0f));

    
    // gradients
    glm::vec3 ga = GetGradient(p + glm::vec3(0.0f, 0.0f, 0.0f));
    glm::vec3 gb = GetGradient(p + glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 gc = GetGradient(p + glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 gd = GetGradient(p + glm::vec3(1.0f, 1.0f, 0.0f));
    glm::vec3 ge = GetGradient(p + glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 gf = GetGradient(p + glm::vec3(1.0f, 0.0f, 1.0f));
    glm::vec3 gg = GetGradient(p + glm::vec3(0.0f, 1.0f, 1.0f));
    glm::vec3 gh = GetGradient(p + glm::vec3(1.0f, 1.0f, 1.0f));
    
    // projections
    float va = glm::dot(ga, w - glm::vec3(0.0f, 0.0f, 0.0f));
    float vb = glm::dot(gb, w - glm::vec3(1.0f, 0.0f, 0.0f));
    float vc = glm::dot(gc, w - glm::vec3(0.0f, 1.0f, 0.0f));
    float vd = glm::dot(gd, w - glm::vec3(1.0f, 1.0f, 0.0f));
    float ve = glm::dot(ge, w - glm::vec3(0.0f, 0.0f, 1.0f));
    float vf = glm::dot(gf, w - glm::vec3(1.0f, 0.0f, 1.0f));
    float vg = glm::dot(gg, w - glm::vec3(0.0f, 1.0f, 1.0f));
    float vh = glm::dot(gh, w - glm::vec3(1.0f, 1.0f, 1.0f));
	
    // interpolation
    return va + 
           u.x * (vb - va) +
           u.y * (vc - va) +
           u.z * (ve - va) +
           u.x * u.y * (va-vb-vc+vd) +
           u.y * u.z * (va-vc-ve+vg) +
           u.z * u.x * (va-vb-ve+vf) +
           u.x * u.y * u.z * (-va+vb+vc-vd+ve-vf-vg+vh);
}
