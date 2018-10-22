#include "octal.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <stack>
#include "samplerfunctions.h"
#include "cubeintersect.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

Octal::Octal(QOpenGLFunctions_4_5_Core *QTGL, Perlin * perlin) : _perlin(perlin)
{
	_shaderTree = new shaderstorage<ShaderOctalNode>(OCTAL_MAX, QTGL);
	//_bricks = new SimpleTexture(QTGL, glm::uvec3(64));

	//FillOctal();
	registerPerlinNoise(perlin);
	//_Root = CreateOctalFromSamplerFunc(perliner, 6, true, false, "perlinWolkjes6.oct"); //Dont do 9+ or sometimes 8+ (Perlin)
	//OctalNode * A = loadOctalTree("perlinSphereBright.oct");
	//OctalNode * A = loadOctalTree("weirdflower9_color_yellow.oct");
	//OctalNode * B = loadOctalTree("bergjes.oct");

	//_Root = A->mergeNodeTree(B);
	//_Root->mergeNodeTree(B);

	//_Root = OctalNode::createFromHeightSampler(hoogte, kleurPerlin_0, 10);//kleurPerlin_0, 10);

	_root = OctalNode::createFromHeightMap(QImage("../Raycaster/Octal/Grond_00.png"), { kleurZwart, kleurZwart, kleurZwart });// kleurGrijsGradient);

	//std::cout << "starting to convert deeper levels to textures!" << std::endl;
	//_root->convertToTex();

	std::cout << "shooting light" << std::endl;
	shootLight();

	std::cout << "starting to convert to shader buffer!" << std::endl;
	uint32_t nodesCreated = ConvertOctalToShader();

	std::cout << "writing buffer of #" << nodesCreated << std::endl;
	_shaderTree->SchrijfWeg(nodesCreated);

	//std::cout << "writing texture" << std::endl;
	//_bricks->createTexture();
}

void Octal::shootLight()
{
	float		phi = 0.0f,
				theta = 0.0f;

	glm::mat4	ModelView = glm::mat4() *
							glm::rotate(phi,   glm::vec3(0.0f, 0.0f, 1.0f)) *
							glm::rotate(theta, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3	Translatie			= glm::vec3(0, 6, 0),
				Ray					= glm::vec3(0.0f, -1.0f, 0.0f),			//normalize(glm::mat3(ModelView) * glm::vec3(0, 0, 1.0)),
				Origin				= Translatie,							//glm::mat3(ModelView) * Translatie,
				TotalCubeBounds[2]	= {glm::vec3(-1.0f), glm::vec3(1.0f)};

	int multi = 2 * 100000; //800000;
	for(int i=0; i< multi * 100; i++)
	{
		do
		{
			Ray = randvec3(-1.0f, 1.0f);
		}
		while(glm::length(Ray) > 1 && Ray.z > 0.0f && Ray.y < 0.0f);

		Ray		 = glm::normalize(Ray);
		Origin	 = glm::vec3(0.0f, 5.0f, -7.0f);

		//std::cout << "Ray " << Ray << "\tand Origin " << Origin << std::endl;

		foton * found = GetCubeIntersectColor(_root, Ray, TotalCubeBounds, Origin, glm::vec4(randvec3(0.0f, 1.0f), 1.0f));//glm::vec4(1.0f));//glm::vec4(glm::vec3(0.5f) + (0.5f * Ray), 1.0f));

		//std::cout << (found != NULL ? "found a cube with intersect!" : "didn't find a cube with intersect..") << std::endl;

		if(found != NULL)
			delete found;
		found = NULL;


		if(i % (multi * 5) == 0)
			std::cout << "light at " << (i / multi) << "%" << std::endl;
	}

}

OctalNode * Octal::FillOctal()
{
    int Counter(1);

	OctalNode * Root = new OctalNode();
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

	return Root;
}

OctalNode * Octal::loadOctalTree(const char * naam)
{
	std::ifstream fileIn;

	std::cout << naam << " open for reading!" << std::endl;
	fileIn.open(naam, std::ios::in | std::ios::binary);
	assert(fileIn.is_open());

	std::cout << naam <<  " extraction!" << std::endl;
	OctalNode * loaded = OctalNode::constructFromStream(fileIn);

	std::cout << naam <<  " closed!" << std::endl;
	fileIn.close();

	return loaded;
}

void Octal::saveAs(OctalNode * tree, std::string name)
{
	std::ofstream fileOut;

	std::cout <<  name <<  " open for writing!" << std::endl;
	fileOut.open(name, std::ios::out | std::ios::binary);
	assert(fileOut.is_open());

	std::cout <<  name << " insertion!" << std::endl;
	tree->insertIntoStream(fileOut);

	std::cout <<  name << " closed!" << std::endl;
	fileOut.close();
}

OctalNode * Octal::CreateOctalFromSamplerFunc(samplerFunc sampler, int Depth, bool shouldGenerateAndSave, bool shouldLoad, const char * naam)
{
	OctalNode * Root = NULL;

	bool	shouldGenerate	= shouldGenerateAndSave,
			shouldSave		= shouldGenerateAndSave;

	if(shouldGenerate)
	{
		Root = new OctalNode();

		std::cout << "Root->createChildForDepthWithSampler(sampler="<<sampler<<", Depth="<<Depth<<");" << std::endl;
		Root->createChildForDepthWithSampler(sampler, Depth);

	//	std::cout << "Root->PruneEmptyChildren();" << std::endl;
	//	Root->PruneEmptyChildren();

	//	std::cout << "Root->MergeFullChildren();" << std::endl;
	//	Root->MergeFullChildren();
	}

	if(shouldSave)
		saveAs(Root, naam);

	if(shouldLoad)
	{
		if(shouldGenerate)
			delete Root;

		return loadOctalTree(naam);
	}

	return Root;
}

uint32_t Octal::ConvertOctalToShader()
{
    uint32_t RunningCounter = 0;
	_octToShadNode.clear();
	ConvertOctalToShader(_root, RunningCounter, OCTAL_MAX, OCTAL_MAX, 0);

    qDebug("Gevonden MaxDepth = %u\n", MaxDepth);

	return RunningCounter;
}

void Octal::printTree()
{
	uint NaamDiepte = 0;
    std::map<OctalNode*, std::string> NodeToNaam;
	std::string Resultaat = printTree(_root, NodeToNaam, NaamDiepte);
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

	(*_shaderTree)[HuidigeIndex].Kleur = glm::mediump_vec4(HuidigeNode->Kleur);


	if(HuidigeNode->_texture != NULL)
	{
		size_t octIndex = _bricks->addOctTex(HuidigeNode->_texture);
		(*_shaderTree)[HuidigeIndex].Sub[0] = OCTAL_MAX + 1;
		(*_shaderTree)[HuidigeIndex].Sub[1] = octIndex;
	}
	else
		for(int i=0; i<8; i++)
			(*_shaderTree)[HuidigeIndex].Sub[i] = HuidigeNode->Sub[i] == NULL ? OCTAL_MAX : ConvertOctalToShader(HuidigeNode->Sub[i], Counter, HuidigeIndex, i, Diepte + 1);

    return HuidigeIndex;
}



