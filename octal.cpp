#include "octal.h"

Octal::Octal(QOpenGLFunctions_4_5_Core *QTGL)
{
    ShaderTree = new shaderstorage<ShaderOctalNode>(OCTAL_MAX, QTGL);

    FillOctal();

    ConvertOctalToShader();
}

void Octal::FillOctal()
{
    int Counter(1);

    Root = new OctalNode();

    OctalNode * Current(Root);
    std::queue<OctalNode*> Stacker;
    Stacker.push(NULL);

    while(Counter < OCTAL_MAX && Current != NULL)
    {
        bool Omlaag = Stacker.front() == NULL || rand()%2 ==0;

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
                {
                    Stacker.push(Current);
                    Current = Current->Sub[RandSub];
                }
            }
            else
            {
                Current->Sub[RandSub] = new OctalNode(glm::vec4(randcolor(), (float)(rand()%2)));
                Counter++;
                printf("Added node %d out of %d\n", Counter, OCTAL_MAX);
            }
        }
        else
        {
            Current = Stacker.front();
            Stacker.pop();
        }

    }
}

void Octal::ConvertOctalToShader()
{
    uint32_t RunningCounter = 0;
    OctalNodeToShaderIndex.clear();
    ConvertOctalToShader(Root, RunningCounter);

    for(int i=0; i<OCTAL_MAX; i++)
        printf("(*ShaderTree)[%d](%1.2f,%1.2f,%1.2f) verwijst naar %u %u %u %u %u %u %u %u\n",i,
            (*ShaderTree)[i].Kleur.x, (*ShaderTree)[i].Kleur.y, (*ShaderTree)[i].Kleur.z,
            (*ShaderTree)[i].Sub[0], (*ShaderTree)[i].Sub[1],  (*ShaderTree)[i].Sub[2],  (*ShaderTree)[i].Sub[3],
            (*ShaderTree)[i].Sub[4], (*ShaderTree)[i].Sub[5],  (*ShaderTree)[i].Sub[6],  (*ShaderTree)[i].Sub[7]);
}

uint32_t Octal::ConvertOctalToShader(OctalNode* HuidigeNode, uint32_t& Counter)
{
    uint32_t HuidigeIndex = Counter;
    Counter++;

    //OctalNodeToShaderIndex[HuidigeNode] = HuidigeIndex;
    (*ShaderTree)[HuidigeIndex].Kleur = glm::mediump_vec4(HuidigeNode->Kleur);

    for(int i=0; i<8; i++)
        (*ShaderTree)[HuidigeIndex].Sub[i] = HuidigeNode->Sub[i] == NULL ? OCTAL_MAX : ConvertOctalToShader(HuidigeNode->Sub[i], Counter);

    return HuidigeIndex;
}
