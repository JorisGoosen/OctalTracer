#include "octalnode.h"
#include <iostream>

const float considerThisEmpty = 0.1;

uint64_t OctalNode::newId()
{
	static uint64_t idSeed = 1;

	return idSeed++;
}

void OctalNode::ZetMij(glm::vec4 DezeKleur, OctalNode * JeOuder)
{
	for(int i=0; i<8; i++)
		Sub[i] = NULL;

	Kleur = DezeKleur;
	Ouder = JeOuder;
}


void OctalNode::createChildForDepthWithSampler(samplerFunc sampler, int Diepte, glm::vec3 Coordinaat, float KubusRadius)
{
	static int count		= 0;
	static int total		= std::pow(8, Diepte);
	static int lastCheck	= -1;
	static int divider		= total / 100;

	if(Diepte == 0)
	{
		Kleur = (*sampler)(Coordinaat);

		if(count++ - lastCheck > divider)
		{
			lastCheck = count - (count % divider);

			double procent = (((double)count) / ((double)total)) * 100.0;
			std::cout << "modelseed at " << procent << "%\n" << std::endl;

		}
	}
	else
		for(int x=0; x<2; x++)
			for(int y=0; y<2; y++)
				for(int z=0; z<2; z++)
				{
					glm::vec3 rel(x==0 ? -1.0f : 1.0f, y==0 ? -1.0f : 1.0f, z==0 ? -1.0f : 1.0f);
					glm::vec3 curCoord(Coordinaat + (rel * KubusRadius * 0.5f));

					uint SubIndex = x + (y * 2) + (z * 4);
					Sub[SubIndex] = new OctalNode(this);
					Sub[SubIndex]->createChildForDepthWithSampler(sampler, Diepte - 1, curCoord, KubusRadius * 0.5f);
				}

	PruneEmptyChildren();
	MergeFullChildren();
}


bool OctalNode::PruneEmptyChildren()
{
	bool allChildrenAreEmpty = true;
	bool IHaveNoChildren = true;

	for(int i=0; i<8; i++)
		if(Sub[i] != NULL)
		{
			IHaveNoChildren = false;

			if(!Sub[i]->PruneEmptyChildren())
				allChildrenAreEmpty = false;
			else
			{
				delete Sub[i];
				Sub[i] = NULL;
			}
		}

	if(IHaveNoChildren)
		return Kleur.a < considerThisEmpty;

	return allChildrenAreEmpty;
}

bool OctalNode::MergeFullChildren()
{
	bool allChildrenAreFull = true;
	int iHaveThisManyChildren = 0;

	glm::vec4 AvgCol(0.0f);

	for(int i=0; i<8; i++)
		if(Sub[i] != NULL)
		{
			iHaveThisManyChildren++;

			if(!Sub[i]->MergeFullChildren())
				allChildrenAreFull = false;

			AvgCol += Sub[i]->Kleur;
		}

	if(iHaveThisManyChildren == 8 && allChildrenAreFull)
	{
		Kleur = AvgCol / 8.0f;

		for(int i=0; i<8; i++)
		{
			delete Sub[i];
			Sub[i] = NULL;
		}

		return Kleur.a > 1.0f - considerThisEmpty; //Should always be true anyway
	}

	return iHaveThisManyChildren == 0 && Kleur.a > 1.0f - considerThisEmpty;
}


void OctalNode::insertIntoStream(std::ofstream& fileOut)
{
	//std::cout << "Inserting id=" << id << std::endl;
	//fileformat = 8bytes(id)	4bytes(rgba)	8*8bytes(indices of children 0 - 7, 0 if no child)

	fileOut.write(reinterpret_cast<char*>(&id), sizeof id);

	for(int i=0; i<4; i++)
	{
		unsigned char rgba = (unsigned char)(Kleur[i] * 255);
		fileOut.write(reinterpret_cast<char*>(&rgba), sizeof rgba);
	}


	for(int i=0; i<8; i++)
	{
		uint64_t getal = Sub[i] == NULL ? 0 : Sub[i]->id;
		fileOut.write(reinterpret_cast<char*>(&getal), sizeof getal);
	}

	for(int i=0; i<8; i++)
		if(Sub[i] != NULL)
			Sub[i]->insertIntoStream(fileOut);

}

OctalNode * OctalNode::constructFromStream(std::ifstream& fileIn)
{
	std::map<uint64_t, std::pair<OctalNode*,size_t>> lookingForLove;
	OctalNode * result = constructFromStream(fileIn, lookingForLove);

	if(lookingForLove.size() > 0)
		std::cout << "lookingForLove has got some (" << lookingForLove.size() << ") nodes remaining.." << std::endl;

	return result;
}

OctalNode * OctalNode::constructFromStream(std::ifstream& fileIn, std::map<uint64_t, std::pair<OctalNode*,size_t> >& lookingForLove)
{
	OctalNode * returnThis = NULL;

	while(fileIn)
	{
		glm::vec4 RGBA;

		uint64_t id;
		fileIn.read(reinterpret_cast<char*>(&id), sizeof id);

		for(int i=0; i<4; i++)
		{
			unsigned char rgba;
			fileIn.read(reinterpret_cast<char*>(&rgba), sizeof rgba);

			RGBA[i] = (float)(rgba) / 255.0f;
		}

		//std::cout << "just extracted id " << id << "  with color: (" << RGBA[0] << ", " <<  RGBA[1] << ", " <<  RGBA[2] << ", " <<  RGBA[3] << ")" << std::endl;

		if(id == 0)
			break;

		OctalNode * nieuwe = new OctalNode(RGBA, id);

		if(returnThis == NULL)
			returnThis = nieuwe;

		if(lookingForLove.count(id) > 0)
		{
			auto mommy = lookingForLove[id];
			mommy.first->ZetSub(mommy.second, nieuwe);
			lookingForLove.erase(id);
		}

		for(size_t i=0; i<8; i++)
		{
			fileIn.read(reinterpret_cast<char*>(&id), sizeof id);
			//std::cout << "id " << nieuwe->id << " contains Sub["<<i<<"] id: " << id << std::endl;

			if(id != 0)
				lookingForLove[id] = std::make_pair(nieuwe, i);
		}
	}

	return returnThis;
}

void OctalNode::ZetSub(size_t num, OctalNode * deze)
{
	//std::cout << "Found Sub["<<num<<"] of id " << id << " namely child: " <<deze->id << std::endl;

	Sub[num] = deze;
	deze->Ouder = this;
}
