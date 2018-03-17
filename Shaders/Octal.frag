#version 440
#extension GL_ARB_shader_storage_buffer_object :   enable
#extension GL_EXT_gpu_shader4 : enable


out vec4 fColor;

//uniform vec3 RPos, GPos, BPos;
//uniform vec3 RGBFragMultiplier;

uniform uint MAXDIEPTE;
uniform uint OCTAL_MAX;

struct ShaderOctalNode
{
    vec4 Kleur;
    uint Sub[8];
};

layout(std430, binding = 0) buffer ShaderTree	{ ShaderOctalNode data[]; } Nodes;

in vec3 Origin;
in vec3 Ray;
in vec3 InvRay;

const vec3 TotalCubeBounds[2] = {vec3(-2.5f), vec3(2.5f)};
vec4 Accumulatie = vec4(0.0f);

vec4 AccumuleerKleur(vec4 Kleur, float Afstand)
{
	float maxAlpha = min(1.0f - Accumulatie.a, Kleur.a);
	Accumulatie += vec4(Kleur.rgb * maxAlpha, maxAlpha);
	return Accumulatie;
}

// http://chiranjivi.tripod.com/octrav.html
vec4 GetCubeIntersectColor()
{


	const vec4 FaalKleur	= vec4(Ray * 0.5f + vec3(0.5f), 1.0f);
	const vec3 InvRay		= vec3(1.0f) / Ray;
	const ivec3 Sign		= ivec3(int(Ray.x < 0), int(Ray.y < 0), int(Ray.z < 0));

	const vec3 CubeMin = vec3(TotalCubeBounds[  Sign.x].x, TotalCubeBounds[  Sign.y].y, TotalCubeBounds[  Sign.z].z);
	const vec3 CubeMax = vec3(TotalCubeBounds[1-Sign.x].x, TotalCubeBounds[1-Sign.y].y, TotalCubeBounds[1-Sign.z].z);

	int Diepte = 0;

	const vec3 original_tmin = (CubeMin - Origin) * InvRay, original_tmax = (CubeMax - Origin) * InvRay;

	float minimumMaxMin = 0.0f, minmax, maxmin;
	vec3 tmin = original_tmin, tmax = original_tmax, tmid;
	const uvec3 AssenMult = uvec3(1, 2, 4);
	uint SubIndex, KindIndex, NodeIndex = 0, PreviousNodeIndex = 0;

	float descends = 1.0f;

	while(Diepte >= 0)
    {
		maxmin = max(minimumMaxMin, max(max(tmin.x, tmin.y), tmin.z));
		minmax = min(min(tmax.x, tmax.y), tmax.z);

		if(maxmin >= minmax || minmax < 0 || maxmin < 0)
			return AccumuleerKleur(FaalKleur, 1) * descends;

		tmid = (tmin + tmax) * 0.5f;

		SubIndex = 0;
		for(int as=0; as<3; as++)
			SubIndex += AssenMult[as] * int(Sign[as] == int(tmid[as] > maxmin));

		KindIndex	= Nodes.data[NodeIndex].Sub[SubIndex];

		if(KindIndex != OCTAL_MAX)
		{
			Diepte++;

			PreviousNodeIndex	= NodeIndex;
			NodeIndex			= KindIndex;

			for(int i=0; i<3; i++)
				if(tmid[i] > maxmin)
					tmax[i] = tmid[i];
				else
					tmin[i] = tmid[i];
		}
		else
		{
			vec4 GevondenNodeKleur =  Nodes.data[PreviousNodeIndex].Kleur;

			if(AccumuleerKleur(GevondenNodeKleur, minmax - maxmin).a > 0.99f)
				return Accumulatie * descends;
			else
			{
				Diepte				= 0;
				NodeIndex			= 0;
				PreviousNodeIndex	= 0;
				minimumMaxMin		= minmax + 0.00000000000000000001f;//max(0.001f, (minmax - maxmin) * 0.001f);
				tmin				= original_tmin;
				tmax				= original_tmax;

				//descends *= 0.95f;
			}
		}
    }

	return vec4(1.0f);
}


void main(void)
{
	fColor = GetCubeIntersectColor();

}
