#ifndef OCTAL_H
#define OCTAL_H

#include "shaderstorage.h"
#include <vector>
#include <algorithm>
#include <map>
//#include <queue>
#include <QOpenGLFunctions_4_5_Core>
#include "commonfunctions.h"

const uint32_t OCTAL_MAX = 8092 * 4;

struct OctalNode
{
    OctalNode(OctalNode * JeOuder = NULL)
    {
        ZetMij(glm::vec4(0.0f), JeOuder);
    }

    OctalNode(glm::vec4 DezeKleur, OctalNode * JeOuder = NULL)
    {
        ZetMij(DezeKleur, JeOuder);
    }

    void ZetMij(glm::vec4 DezeKleur, OctalNode * JeOuder)
    {
        for(int i=0; i<8; i++)
            Sub[i] = NULL;

        Kleur = DezeKleur;
        Ouder = JeOuder;
    }

    glm::vec4 Kleur;
    OctalNode * Sub[8];
    OctalNode * Ouder = NULL;

	///returns true if everything below is empty
	bool PruneEmptyChildren();
};

struct ShaderOctalNode
{
	glm::mediump_vec4 Kleur;
	//float Kleur[4];
    uint32_t Sub[8];
    //uint32_t Ouder, SubIndex;
    //uint32_t Padding[2];
};

class Octal
{
public:
    Octal(QOpenGLFunctions_4_5_Core *QTGL);

    void BindBuffer(int BindOctalTree = 0) {	ShaderTree->BindBufferStorage(BindOctalTree);	}
    void FillOctal();

    void ConvertOctalToShader();
    uint32_t MaxDepth = 0;

    void printTree();

private:
	std::string printTree(OctalNode * HuidigeNode, std::map<OctalNode*, std::string> & NodeToNaam, uint & NaamDiepte, std::string InSpring = "");

    uint32_t ConvertOctalToShader(OctalNode * HuidigeNode, uint32_t &Counter, uint32_t Ouder, uint32_t SubIndex, uint32_t Diepte);

    std::map<OctalNode, uint32_t> OctalNodeToShaderIndex;
    shaderstorage<ShaderOctalNode> *ShaderTree;
    OctalNode * Root;
};

#endif // OCTAL_H
