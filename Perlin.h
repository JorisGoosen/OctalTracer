#pragma once

#include "shaderstorage.h"
#include <QVector3D>
#include <vector>
#include <list>
#include <algorithm>
#include <QtCore/qmath.h>


#define PERLIN_NUM_GRADIENTS 1024

class Perlin
{
public:
                Perlin(QOpenGLFunctions_4_5_Core *QTGL);
				~Perlin();
	void		Init();
	void		BindBuffers(int BindIndexGradient = 0, int BindIndexPermutations = 1){
					RandomGradients->BindBufferStorage(BindIndexGradient);
					PermutationTable->BindBufferStorage(BindIndexPermutations);
				}
		
    glm::mediump_vec3   GetGradient(glm::ivec3 loc);
    float 		GetIniqoQuilesNoise(glm::vec3 x);
	
private:
	void		InitGradients();
	void		InitPermutationTable();
    int			FoldingFunction(glm::ivec3 ijk) { return (*PermutationTable)[(*PermutationTable)[(*PermutationTable)[ijk[0]] + ijk[1]] + ijk[2]]; }

    shaderstorage<glm::mediump_vec3> *RandomGradients = NULL;
    shaderstorage<int>			*PermutationTable = NULL;
    QOpenGLFunctions_4_5_Core *GL;
};


