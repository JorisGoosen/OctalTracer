#ifndef OCTAL_H
#define OCTAL_H

#include "shaderstorage.h"
#include <vector>
#include <algorithm>
#include <map>
#include <queue>
#include <QOpenGLFunctions_4_5_Core>
#include "commonfunctions.h"

#define OCTAL_MAX 4096

struct OctalNode
{
    OctalNode()
    {
        ZetMij(glm::vec4(0.0f));
    }

    OctalNode(glm::vec4 DezeKleur)
    {
        ZetMij(DezeKleur);
    }

    void ZetMij(glm::vec4 DezeKleur)
    {
        for(int i=0; i<8; i++)
            Sub[i] = NULL;

        Kleur = DezeKleur;
    }

    glm::vec4 Kleur;
    OctalNode * Sub[8];
};

struct ShaderOctalNode
{
    glm::mediump_vec4 Kleur;
    uint32_t Sub[8];
};

class Octal
{
public:
    Octal(QOpenGLFunctions_4_5_Core *QTGL);

    void BindBuffer(int BindOctalTree = 0) {	ShaderTree->BindBufferStorage(BindOctalTree);	}
    void FillOctal();

    void ConvertOctalToShader();

private:
    uint32_t ConvertOctalToShader(OctalNode * HuidigeNode, uint32_t &Counter);

    std::map<OctalNode, uint32_t> OctalNodeToShaderIndex;
    shaderstorage<ShaderOctalNode> *ShaderTree;
    OctalNode * Root;
};

#endif // OCTAL_H
