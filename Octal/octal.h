#ifndef OCTAL_H
#define OCTAL_H

#include "shaderstorage.h"
#include <vector>
#include <algorithm>
#include <map>
//#include <queue>
#include <QOpenGLFunctions_4_5_Core>
#include "commonfunctions.h"
#include "Perlin.h"
#include "octalnode.h"

const uint32_t OCTAL_MAX = 1024 * 1024 * 48;

class Octal
{
public:
	Octal(QOpenGLFunctions_4_5_Core *QTGL, Perlin * perlin);

    void BindBuffer(int BindOctalTree = 0) {	ShaderTree->BindBufferStorage(BindOctalTree);	}
	OctalNode * FillOctal();
	OctalNode * CreateOctalFromSamplerFunc(samplerFunc sampler, int Depth, bool shouldGenerateAndSave, bool shouldLoad, const char * naam);
	OctalNode * loadOctalTree(const char * naam);

    void ConvertOctalToShader();
    uint32_t MaxDepth = 0;

    void printTree();
	static void saveAs(OctalNode * tree, std::string name);

private:
	std::string printTree(OctalNode * HuidigeNode, std::map<OctalNode*, std::string> & NodeToNaam, uint & NaamDiepte, std::string InSpring = "");

    uint32_t ConvertOctalToShader(OctalNode * HuidigeNode, uint32_t &Counter, uint32_t Ouder, uint32_t SubIndex, uint32_t Diepte);

    std::map<OctalNode, uint32_t> OctalNodeToShaderIndex;
    shaderstorage<ShaderOctalNode> *ShaderTree;
	OctalNode * _Root;
	Perlin * perlin;
};

#endif // OCTAL_H
