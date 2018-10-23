#version 440
#extension GL_ARB_shader_storage_buffer_object :   enable
#extension GL_EXT_gpu_shader4 : enable

layout(binding = 0) uniform sampler3D bricks;

out vec4 fColor;

uniform vec3 TexSize;
uniform vec3 TexInv;
in vec3 Origin;
in vec3 Ray;
in vec3 InvRay;

const vec3  TotalCubeBounds[2] = {vec3(-2.5f), vec3(2.5f)},
            CubeSize            = TotalCubeBounds[1] - TotalCubeBounds[0];
const ivec3 NextCell[2]         = {ivec3(-1), ivec3(1)};

vec4 Accumulatie = vec4(0.0f);

vec4 AccumuleerKleur(vec4 Kleur, float Afstand)
{
	const float borderForAlpha = 0.9f;
	const float remainderMult = 1.0f / (1.0f - borderForAlpha);

	if(Kleur.a >  borderForAlpha)
		Afstand = mix(Afstand, 1.0f, (Kleur.a - borderForAlpha) * remainderMult);

	float maxAlpha = min(1.0f - Accumulatie.a, Kleur.a * Afstand);
	Accumulatie += vec4(Kleur.rgb * maxAlpha, maxAlpha);
	return Accumulatie;
}

vec3 achtergrondKleur(vec3 Ray)
{
  const float pi = 3.142f;
  const float wittigBegin = 0.9f,   wittigLengte = 1.0f - wittigBegin,
              blauwBegin  = 0.2f,    blauwLengte = 1.0f - blauwBegin;

  const vec3  wittig      = vec3(1.0f, 1.0f, 1.0f),
              blauw       = vec3(0.0f, 0.0f, 1.0f),
              zwart       = vec3(0.0f);

  float       hoogte      = 0.5f * (1.0f + Ray.y);//0.5f + (Ray.y * 0.5f);
  if( hoogte > wittigBegin )
       return mix(blauw, wittig, (hoogte - wittigBegin) / wittigLengte );
  else if( hoogte > blauwBegin)
    return mix(zwart, blauw,  (hoogte - blauwBegin) / blauwLengte);
  return zwart;
}

vec3 getTexUnitPos(vec3 WorldPos)
{
  WorldPos -= TotalCubeBounds[0];
  WorldPos /= CubeSize;

  return WorldPos;
}

vec3 fromTexUnitPos(vec3 TexPos)
{
  TexPos *= CubeSize;
  TexPos += TotalCubeBounds[0];

  return TexPos;
}

// http://chiranjivi.tripod.com/octrav.html
vec4 GetCubeIntersectColor()
{
  const uvec3 AssenMult = uvec3(1, 2, 4);

  const vec4 FaalKleur	= vec4(normalize(Ray), 1.0f);//vec4(achtergrondKleur(Ray), 1.0f); //vec4(0.0f); //vec4(Ray * 0.5f + vec3(0.5f), 1.0f);
  const vec3 InvRay     = vec3(1.0f) / Ray;
  const ivec3 Sign      = ivec3(int(Ray.x < 0), int(Ray.y < 0), int(Ray.z < 0)),
              Step      = ivec3(NextCell[1-Sign.x].x, NextCell[1-Sign.y].y, NextCell[1-Sign.z].z);

  const vec3  CubeMin   = vec3(TotalCubeBounds[  Sign.x].x, TotalCubeBounds[  Sign.y].y, TotalCubeBounds[  Sign.z].z),
              CubeMax   = vec3(TotalCubeBounds[1-Sign.x].x, TotalCubeBounds[1-Sign.y].y, TotalCubeBounds[1-Sign.z].z),
              oritmin   = (CubeMin - Origin) * InvRay,
              oritmax   = (CubeMax - Origin) * InvRay;

  float minmax,
        maxmin;
  vec3  tmin            = oritmin,
        tmax            = oritmax;


  int stop = 10000;

  while(stop-- > 0)
  {
    minmax = min(min(tmax.x, tmax.y), tmax.z);
    maxmin = max(max(tmin.x, tmin.y), tmin.z);

    if(maxmin >= minmax || maxmin < 0)
       return FaalKleur;//AccumuleerKleur(FaalKleur, 1);

    vec3  worldInP  = Origin + (Ray * maxmin),
          texPosIn  = getTexUnitPos(worldInP);


    vec4 accu = vec4(0.0f);

    while(maxmin < minmax)
    {
      const float step = 0.01f;
      maxmin += step;

      if(maxmin < minmax)
      {
        worldInP  = Origin + (Ray * maxmin);
        texPosIn  = getTexUnitPos(worldInP);

        vec4 tmp = texture(bricks, texPosIn);

        if(tmp.a > 0.99f)
          return tmp;

        //accu += TexSize.x * step * vec4((tmp.rgb * tmp.a), tmp.a);

        //if(accu.a > 0.99f)
          //return accu;
      }
    }

    return (accu * accu.a) + (FaalKleur * (1.0f - accu.a));
/*
    //De kubus is altijd precies ge-aligned met dezelfde ruimte als waar de ray in zit
    //Dus hoef ik die alleen maar te volgen tot de eerste botsing een volgende cell
    vec3 texCoord     = floor(texPosIn * TexSize);

    vec4  texColIn    = texture(bricks, texPosIn, 0);
    vec3 nextCoord    = texCoord + Step;

    vec3  outCubeMax  = fromTexUnitPos(nextCoord * TexInv),
          tOut        = (outCubeMax - Origin) * InvRay;

    float celminmax = min(min(tOut.x, tOut.y), tOut.z);

    if(celminmax >= minmax || celminmax < 0)
      return FaalKleur;//AccumuleerKleur(FaalKleur, 1);

    if(texColIn.a > 0.5f)
      return texColIn;

    //float distInCell = 1.0f;//celminmax - maxmin;
    //sif(AccumuleerKleur(texColIn, distInCell).a > 0.99f)
    //  return Accumulatie;

    //maxmin            = max(max(tOut.x, tOut.y), tOut.z);
    tmin = tOut;
*/
    //minimumMaxMin = minmax + 0.00000000000000000001f;

    //return texColIn * texColIn.a;

    //return vec4();
    /*

		tmid = (tmin + tmax) * 0.5f;

		if(NodeIndex != OCTAL_MAX)
		{

			Diepte++;

			SubIndex = 0;
			for(int as=0; as<3; as++)
				SubIndex += AssenMult[as] * int(Sign[as] == int(tmid[as] > maxmin));

			PreviousNodeIndex	= NodeIndex;
			NodeIndex			= Nodes.data[PreviousNodeIndex].Sub[SubIndex];

			for(int i=0; i<3; i++)
				if(tmid[i] > maxmin)
					tmax[i] = tmid[i];
				else
					tmin[i] = tmid[i];
		}
		else
		{
			//return vec4(0.0f, 0.0f, 1.0f, 1.0f);
			vec4 GevondenNodeKleur =  Nodes.data[PreviousNodeIndex].Kleur;

			if(AccumuleerKleur(GevondenNodeKleur, minmax - maxmin).a > 0.99f)
				return Accumulatie * descends;
			else
			{
        minimumMaxMin = minmax + 0.00000000000000000001f;
        tmin          = oritmin;
        tmax          = oritmax;

				//descends *= 0.975f;
			}
    }*/
	//	else if(PreviousNodeIndex == OCTAL_MAX)
		//	return vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	return vec4(1.0f);
}


void main(void)
{
  //fColor = vec4((Ray), 1.0f);

  fColor = GetCubeIntersectColor();
}
