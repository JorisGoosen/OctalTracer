#include "octal.h"
#include <sstream>
#include <iostream>
#include <stack>


glm::vec4 simpleSampler(glm::vec3 coord)
{
	float innerDist = glm::length(coord) - 0.75f;

	float alpha  =  glm::clamp(-innerDist, 0.0f, 0.05f) * 20.0f;

	if(alpha < 0.5f)
		alpha = 0.0f;
	else
		alpha = 1.0f;

	//if(outerDist > 0 || innerDist < 0)
		return glm::vec4((coord * 0.5f) + 0.5f, alpha);
}

glm::vec4 simpleSamplerInverted(glm::vec3 coord)
{
	if(glm::length(coord) > 1.1f)
		return glm::vec4(randcolor(), 1.0f);
	return glm::vec4(0.0f);
}


glm::vec4 doubleWhammy(glm::vec3 coord)
{
	float outerDist = glm::length(coord) - 1.15f;
	float innerDist = glm::length(coord) - 0.75f;

	float alpha  =  glm::clamp(outerDist > 0 ? outerDist : -innerDist, 0.0f, 0.05f) * 20.0f;

	//if(outerDist > 0 || innerDist < 0)
		return glm::vec4((coord * 0.5f) + 0.5f, alpha);
	//return glm::vec4(0.0f);
}

glm::vec4 perlinNoise(glm::vec3 coord)
{
	//if( > 0.5)
		//return glm::vec4(, 1.0f);
	float noiseVal0 = 0.75 * Perlin::thePerlin()->GetIniqoQuilesNoise(coord * 0.025f);
	float noiseVal1 = Perlin::thePerlin()->GetIniqoQuilesNoise(glm::vec3(0.5f, 2.1f, 5.321f) + (coord * 0.1f));
	float noiseMax0 = glm::max(noiseVal0, noiseVal1);
	if(noiseMax0 < 0.5f) noiseMax0 = 0.0f;

	return glm::vec4((coord * 0.5f) + 0.5f, glm::clamp(noiseMax0, 0.0f, 1.0f));
}

glm::vec3 randOffset;

glm::vec4 detailPerlinNoise(glm::vec3 coord)
{
    float noiseVal0 = Perlin::thePerlin()->GetIniqoQuilesNoise(randOffset + (coord * 0.1f));


    return glm::vec4((coord * 0.5f) + 0.5f, glm::clamp(glm::pow(noiseVal0, 2.0f), 0.0f, 1.0f));
}

glm::vec4 combine(glm::vec3 coord)
{
	glm ::vec4 a = simpleSampler(coord);
	glm ::vec4 b = detailPerlinNoise(coord);

	glm::vec3 rgb = glm::vec3(1.0f);
	float alpha = glm::min(a.a, b.a);
	return glm::vec4(glm::vec3(perlinNoise(coord.xyz).a, perlinNoise(coord.zxy).a, perlinNoise(coord.zyx).a) , alpha > 0.5f ? 1.0f : 0.0f);
}


Octal::Octal(QOpenGLFunctions_4_5_Core *QTGL)
{
    ShaderTree = new shaderstorage<ShaderOctalNode>(OCTAL_MAX, QTGL);

	randOffset = randvec3(-1.0f, 1.0f);

	//FillOctal();
	CreateOctalFromSamplerFunc(combine, 7); //Dont do 9+ or sometimes 8+ (Perlin)
	//CreateOctalFromSamplerFunc(simpleSampler, 6);
    ConvertOctalToShader();

    ShaderTree->SchrijfWeg();
}

void Octal::FillOctal()
{
    int Counter(1);

    Root = new OctalNode();
    Root->Kleur = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);

    OctalNode * Current(Root);

    //int MaxLeeg = 6;

	int Diepte = 0;

	glm::vec3 Coordinaat(0.0f);
	float KubusRadius = 1.0f; //1 in iedere richting
	std::stack<glm::vec3> vorigeCoordinaten;

    while(Counter < OCTAL_MAX && Current != NULL)
    {
		bool ThereIsOneSeeThrough = false;
		for(int i=0; i<8; i++)
			if(Current->Sub[i] == NULL || Current->Sub[i]->Kleur.w == 0.0f)
				ThereIsOneSeeThrough = true;

		bool Omlaag = Current->Ouder == NULL || (rand()%2 ==0 && ThereIsOneSeeThrough);

        if(Omlaag)
        {
            int RandSub = rand()%8;

			int x = RandSub%2, y = (RandSub-x)%4 > 0, z=(RandSub-x-(y*4))%8 >0;
			glm::vec3 rel(x==0 ? -1.0f : 1.0f, y==0 ? -1.0f : 1.0f, z==0 ? -1.0f : 1.0f);

			glm::vec3 curCoord(Coordinaat + (rel * KubusRadius * 0.5f));
			const float sphereRad = 0.85f;



            if(Current->Sub[RandSub] != NULL)
            {

                if(Current->Sub[RandSub]->Kleur.w == 0.0f)
				{
                    Current = Current->Sub[RandSub];
					Diepte++;

					vorigeCoordinaten.push(Coordinaat);


					KubusRadius *= 0.5f;
					Coordinaat = curCoord;

				}
            }
            else
            {
				//bool Leeg = RandSub < 7 && RandSub > 0;
				//bool Leeg = RandSub == Diepte % 8;
				bool Leeg = Diepte < 3 || glm::length(curCoord) > sphereRad;
				//bool Leeg = Diepte < 4 || rand()%6 != 0;
				//bool Leeg = Diepte < 5 || rand()%4 != 0;
				Current->Sub[RandSub] = new OctalNode(glm::vec4(randcolor(), Leeg ? 0.0f : 1.0f), Current);
				//Current->Sub[RandSub] = new OctalNode(glm::vec4(0.0f, 0.0f, 1.0f, Leeg ? 0.0f : 1.0f), Current);
                Counter++;
                //printf("Added node %d out of %d\n", Counter, OCTAL_MAX);
            }
        }
        else
		{
            Current = Current->Ouder;
			Diepte--;
			Coordinaat = vorigeCoordinaten.top();
			vorigeCoordinaten.pop();
			KubusRadius *= 2.0f;
		}
    }

	Root->PruneEmptyChildren();
}

void OctalNode::createChildForDepthWithSampler(samplerFunc sampler, int Diepte, glm::vec3 Coordinaat, float KubusRadius)
{
	if(Diepte == 0)
		Kleur = (*sampler)(Coordinaat);
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
}




void  Octal::CreateOctalFromSamplerFunc(samplerFunc sampler, int Depth)
{
	Root = new OctalNode();

	Root->createChildForDepthWithSampler(sampler, Depth);

	Root->PruneEmptyChildren();
	Root->MergeFullChildren();
}

const float considerThisEmpty = 0.1;

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

void Octal::ConvertOctalToShader()
{
    uint32_t RunningCounter = 0;
    OctalNodeToShaderIndex.clear();
    ConvertOctalToShader(Root, RunningCounter, OCTAL_MAX, OCTAL_MAX, 0);

    qDebug("Gevonden MaxDepth = %u\n", MaxDepth);
}

void Octal::printTree()
{
	uint NaamDiepte = 0;
    std::map<OctalNode*, std::string> NodeToNaam;
	std::string Resultaat = printTree(Root, NodeToNaam, NaamDiepte);
	std::cout << Resultaat << std::flush;

	//printf("%s\n", Resultaat.c_str());
}

std::string GenereerNaam(uint NaamDiepte)
{
    std::string Naam = "";

    int WerkDiepte = NaamDiepte;
    bool GaDoor = true;

    while(GaDoor)
    {
        uint SpecifiekeLetter = WerkDiepte % 26;
        char Base = 'A' + SpecifiekeLetter;

        Naam = Naam + Base;

        WerkDiepte -= SpecifiekeLetter;
		if(WerkDiepte <= 0)
            GaDoor = false;

        WerkDiepte /= 26;
     }

    return Naam;
}


std::string Octal::printTree(OctalNode * HuidigeNode, std::map<OctalNode*, std::string> & NodeToNaam, uint & NaamDiepte, std::string InSpring)
{
    std::string DezeNaam = GenereerNaam(NaamDiepte);
	NaamDiepte++;

    if(NodeToNaam.count(HuidigeNode) > 0)
        printf("ERROR! Zelfde node komt twee keer voor.... Namelijk: %s\n", NodeToNaam[HuidigeNode].c_str());
    else
        NodeToNaam[HuidigeNode] = DezeNaam;

	std::stringstream ReturnThis;
	ReturnThis << InSpring << "Node " << DezeNaam << " (";
	ReturnThis << ((int)(HuidigeNode->Kleur.x * 255)) << ", "<< ((int)(HuidigeNode->Kleur.y * 255))<<", " <<((int)(HuidigeNode->Kleur.z * 255))<<", " <<((int)(HuidigeNode->Kleur.a * 255))<<"): \n";


	for(int i=0; i<8; i++)
		ReturnThis << (HuidigeNode->Sub[i] == NULL ? "" : printTree(HuidigeNode->Sub[i], NodeToNaam, NaamDiepte, InSpring + "\t"));

	return ReturnThis.str();
}

uint32_t Octal::ConvertOctalToShader(OctalNode* HuidigeNode, uint32_t& Counter, uint32_t Ouder, uint32_t SubIndex, uint32_t Diepte)
{
    MaxDepth = glm::max(Diepte, MaxDepth);

    uint32_t HuidigeIndex = Counter;
    Counter++;

    //(*ShaderTree)[HuidigeIndex].Ouder = Ouder;
    //(*ShaderTree)[HuidigeIndex].SubIndex = SubIndex;;

	(*ShaderTree)[HuidigeIndex].Kleur = glm::mediump_vec4(HuidigeNode->Kleur);


    for(int i=0; i<8; i++)
        (*ShaderTree)[HuidigeIndex].Sub[i] = HuidigeNode->Sub[i] == NULL ? OCTAL_MAX : ConvertOctalToShader(HuidigeNode->Sub[i], Counter, HuidigeIndex, i, Diepte + 1);

    return HuidigeIndex;
}
