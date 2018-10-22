#ifndef OCTALNODE_H
#define OCTALNODE_H

#include <vector>
#include <algorithm>
#include <map>
#include "commonfunctions.h"
#include <fstream>
#include <QImage>
//#include "Perlin.h"
#include "octaltex.h"

#include "samplerfuncdefs.h"

class OctalNode
{
public:
	OctalNode(OctalNode * JeOuder = NULL) : id(newId())
	{
		ZetMij(glm::vec4(0.0f), JeOuder);
	}

	OctalNode(glm::vec4 DezeKleur, OctalNode * JeOuder = NULL) : id(newId())
	{
		ZetMij(DezeKleur, JeOuder);
	}

	OctalNode(glm::vec4 DezeKleur, uint64_t id) : id(id)
	{
		ZetMij(DezeKleur, NULL);
	}

	~OctalNode()
	{
		for(auto & sub : Sub)
			if(sub != NULL)
			{
				delete sub;
				sub = NULL;
			}

		if(_texture != NULL)
			delete _texture;
		_texture = NULL;
	}

	static uint64_t newId();
	void ZetMij(glm::vec4 DezeKleur, OctalNode * JeOuder);

	void ZetSub(size_t num, OctalNode * deze);

	void createChildForDepthWithSampler(samplerFunc sampler, int Diepte, glm::vec3 Coordinaat = glm::vec3(0.0f), float KubusRad = 1.0f);

	void insertIntoStream(std::ofstream& fileOut);

	static OctalNode * constructFromStream(std::ifstream& fileIn, std::map<uint64_t, std::pair<OctalNode*,size_t> >& lookingForLove);
	static OctalNode * constructFromStream(std::ifstream& fileIn);
	static OctalNode * createFromHeightSampler(heightSamplerFunc heightFunc, colorSamplerFunc colorFunc, int diepte);
	static OctalNode * createFromHeightMap(QImage heightMap, std::vector<colorSamplerFunc> colorFuncs);
	static OctalNode * createFromHeightVec(const std::vector<std::vector<glm::vec4>> & map, std::vector<colorSamplerFunc> colorFuncs, int diepte);



	///returns true if everything below is empty
	bool PruneEmptyChildren();

	///reeturns true if everything below it is a child that is visible
	bool MergeFullChildren();
	uint64_t id;

	bool		iAmFull();
	bool		iAmEmpty();
	bool		iHaveChildren();
	bool		iAmTransparent()	{ return !iAmOpaque();; }
	bool		iAmOpaque()			{ return Kleur.a >= 0.75f; }
	OctalNode * mergeNodeTree(OctalNode * & other);

	void		insertNode(glm::vec4 Kleur, glm::vec3 pos, int diepte, glm::vec3 Coordinaat = glm::vec3(0.0f), float KubusRad = 1.0f);

	static	size_t	toIndex(glm::uvec3 p);
			size_t	convertToTex();
			void	addYourselfToTex(OctalTex * tex, glm::uvec3 texOrigin, uint texRange);


	glm::vec4 Kleur;
	OctalNode * Sub[8];
	OctalNode * Ouder = NULL;

	OctalTex	*_texture = NULL;

};

struct ShaderOctalNode
{
	glm::mediump_vec4 Kleur;
	//float Kleur[4];
	uint32_t Sub[8];
	//uint32_t Ouder, SubIndex;
	//uint32_t Padding[2];
};


#endif // OCTALNODE_H
