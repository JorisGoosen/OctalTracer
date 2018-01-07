#include "octal.h"
#include <sstream>
#include <iostream>

Octal::Octal(QOpenGLFunctions_4_5_Core *QTGL)
{
    ShaderTree = new shaderstorage<ShaderOctalNode>(OCTAL_MAX, QTGL);

    FillOctal();

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

    while(Counter < OCTAL_MAX && Current != NULL)
    {
        bool Omlaag = Current->Ouder == NULL || rand()%2 ==0;

        if(Omlaag)
        {
            bool ThereIsOneSeeThrough = false;
            for(int i=0; i<8; i++)
                if(Current->Sub[i] == NULL || Current->Sub[i]->Kleur.w == 0.0f)
                    ThereIsOneSeeThrough = true;

            int RandSub = rand()%8;

            if(Current->Sub[RandSub] != NULL)
            {
                if(!ThereIsOneSeeThrough)
                    Current->Sub[RandSub]->Kleur.w = 0.0f; //Anders blijven we vastzitten..

                if(Current->Sub[RandSub]->Kleur.w == 0.0f)
                    Current = Current->Sub[RandSub];
            }
            else
            {
                bool Leeg = RandSub < 7 && RandSub > 0;
				Current->Sub[RandSub] = new OctalNode(glm::vec4(randcolor(), Leeg ? 0.0f : 1.0f), Current);
				//Current->Sub[RandSub] = new OctalNode(glm::vec4(0.0f, 0.0f, 1.0f, Leeg ? 0.0f : 1.0f), Current);
                Counter++;
                //printf("Added node %d out of %d\n", Counter, OCTAL_MAX);
            }
        }
        else
            Current = Current->Ouder;
    }
}

void Octal::ConvertOctalToShader()
{
    uint32_t RunningCounter = 0;
    OctalNodeToShaderIndex.clear();
    ConvertOctalToShader(Root, RunningCounter, OCTAL_MAX, OCTAL_MAX, 0);

    qDebug("Gevonden MaxDepth = %u\n", MaxDepth);

    if(false)
        for(int i=0; i<OCTAL_MAX; i++)
            printf("(*ShaderTree)[%d](%1.2f,%1.2f,%1.2f) verwijst naar %u %u %u %u %u %u %u %u\n",i,
                (*ShaderTree)[i].Kleur.x, (*ShaderTree)[i].Kleur.y, (*ShaderTree)[i].Kleur.z,
                (*ShaderTree)[i].Sub[0], (*ShaderTree)[i].Sub[1],  (*ShaderTree)[i].Sub[2],  (*ShaderTree)[i].Sub[3],
                (*ShaderTree)[i].Sub[4], (*ShaderTree)[i].Sub[5],  (*ShaderTree)[i].Sub[6],  (*ShaderTree)[i].Sub[7]);
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
	NaamDiepte++;

    std::string DezeNaam = GenereerNaam(NaamDiepte);

    if(NodeToNaam.count(HuidigeNode) > 0)
        printf("ERROR! Zelfde node komt twee keer voor.... Namelijk: %s\n", NodeToNaam[HuidigeNode].c_str());
    else
        NodeToNaam[HuidigeNode] = DezeNaam;

	std::stringstream ReturnThis;
	ReturnThis << InSpring << "Node " << DezeNaam << " (";
	ReturnThis << ((int)(HuidigeNode->Kleur.x * 255)) << ", "<< ((int)(HuidigeNode->Kleur.y * 255))<<", " <<((int)(HuidigeNode->Kleur.z * 255))<<"): \n";


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
