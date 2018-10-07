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
	//return false;

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

bool OctalNode::iHaveChildren()
{
	for(OctalNode * Kind : Sub)
		if(Kind != NULL)
			return true;

	return false;
}

bool OctalNode::iAmFull()
{
	return !iHaveChildren()	|| Kleur.a >= 0.5f;
}

bool OctalNode::iAmEmpty()
{
	return !iHaveChildren()	|| Kleur.a < 0.5f;
}

//#define PRINT_MERGE_INFO

OctalNode * OctalNode::mergeNodeTree(OctalNode * & other)
{
	#ifdef PRINT_MERGE_INFO
	std::cout << "mergeNodeTree" << std::endl;
	#endif

	if(other == NULL)
	{
		#ifdef PRINT_MERGE_INFO
		std::cout << "other == null!" << std::endl;
		#endif

		return this;
	}

	if(!iHaveChildren())
	{
		#ifdef PRINT_MERGE_INFO
		std::cout << "i have no children" << std::endl;
		#endif

		if(other->iHaveChildren())
		{
			#ifdef PRINT_MERGE_INFO
			std::cout << "other has children" << std::endl;
			#endif

			if(iAmOpaque())
			{
				#ifdef PRINT_MERGE_INFO
				std::cout << "i am opaque" << std::endl;
				#endif

				delete other;
				other = NULL;
				return this;
			}
			else
			{
				#ifdef PRINT_MERGE_INFO
				std::cout << "i am transparent" << std::endl;
				#endif

				other->Ouder = Ouder;
				delete this;
				return other;
			}
		}
		else
		{
			#ifdef PRINT_MERGE_INFO
			std::cout << "other has no children, alpha winner takes all" << std::endl;
			#endif

			if(Kleur.a < other->Kleur.a)
				Kleur = other->Kleur;

			//Kleur = 0.5f * ( Kleur + other->Kleur );

			delete other;
			other = NULL;
			return this;
		}
	}
	else // I Have Children
	{
		#ifdef PRINT_MERGE_INFO
		std::cout << "i have children" << std::endl;
		#endif

		if(!other->iHaveChildren())
		{
			#ifdef PRINT_MERGE_INFO
			std::cout << "other has no children" << std::endl;
			#endif

			if(other->iAmOpaque())
			{
				#ifdef PRINT_MERGE_INFO
				std::cout << "other is opaque so it replaces me" << std::endl;
				#endif

				other->Ouder = Ouder;
				delete this;
				return other;
			}
			else
			{
				#ifdef PRINT_MERGE_INFO
				std::cout << "other is transparent so i delete it" << std::endl;
				#endif

				delete other;
				other = NULL;
				return this;
			}
		}
		else // other has children
		{
			#ifdef PRINT_MERGE_INFO
			std::cout << "other has children as well, lets merge em" << std::endl;
			#endif

			float mijnKinderen = 0, zijnKinderen = 0;

			for(int sub=0; sub < 8; sub++)
			{
				if(Sub[sub] != NULL)
					mijnKinderen++;

				if(other->Sub[sub] != NULL)
					zijnKinderen++;

				if(Sub[sub] != NULL)
					Sub[sub] = Sub[sub]->mergeNodeTree(other->Sub[sub]);
				else if(other->Sub[sub] != NULL)
				{
					Sub[sub]		= other->Sub[sub];
					Sub[sub]->Ouder = this;
					other->Sub[sub] = NULL;
				}
			}

			Kleur = ((Kleur * mijnKinderen) + (other->Kleur * zijnKinderen)) / (mijnKinderen + zijnKinderen);

			delete other;
			other = NULL;

			return this;
		}
	}
}

void OctalNode::insertNode(glm::vec4 newKleur, glm::vec3 pos, int diepte, glm::vec3 Coordinaat, float KubusRad)
{
	if(diepte == 0)
		Kleur = newKleur;
	else
	{
		int xyz[3];
		for(int i=0; i<3; i++)
			xyz[i] = pos[i] < Coordinaat[i] ? 0 : 1;
		int sub = xyz[0] + (2 * xyz[1]) + (4 * xyz[2]);

		if(Sub[sub] == NULL)
			Sub[sub] = new OctalNode(this);

		glm::vec3 rel(	xyz[0] == 0 ? -1.0f : 1.0f,
						xyz[1] == 0 ? -1.0f : 1.0f,
						xyz[2] == 0 ? -1.0f : 1.0f);
		glm::vec3 curCoord(Coordinaat + (rel * KubusRad * 0.5f));

		Sub[sub]->insertNode(newKleur, pos, diepte - 1, curCoord, KubusRad * 0.5f);
	}
}

OctalNode * OctalNode::createFromHeightSampler(heightSamplerFunc heightFunc, int diepte)
{
	OctalNode * Wortel = new OctalNode(NULL);

	float dim = pow(2, diepte);

	float	total = dim * dim,
			lastStep = 0,
			percent = total / 100.0f,
			step = 0;

	for(float x=0; x<dim; x++)
		for(float y=0; y<dim; y++)
		{
			glm::vec4 kleur;
			glm::vec3 pos;
			pos.x = -1.0f + ((x / dim) * 2.0f);
			pos.z = -1.0f + ((y / dim) * 2.0f);
			pos.y = heightFunc(pos.x, pos.z, &kleur);
			Wortel->insertNode(kleur, pos, diepte);

			step++;

			if(step > lastStep + percent)
			{
				lastStep = step;
				std::cout << "Currently at " << step / total * 100.0f << "%" << std::endl;
			}

		}

	return Wortel;

}
