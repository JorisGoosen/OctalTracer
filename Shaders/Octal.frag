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
in vec3 curraydir;

const vec3 TotalCubeBounds[2] = {vec3(-2.5f), vec3(2.5f)};

// http://chiranjivi.tripod.com/octrav.html

vec4 GetCubeIntersectColor(vec3 Begin, vec3 Ray)
{
	const vec4 FaalKleur  = vec4(Ray * 0.5f + vec3(0.5f), 1.0f);

	const vec3 InvRay         = vec3(1.0f) / Ray;
	const ivec3 Sign    = ivec3(int(Ray.x < 0), int(Ray.y < 0), int(Ray.z < 0));

	const vec3 CubeMin = vec3(TotalCubeBounds[  Sign.x].x, TotalCubeBounds[  Sign.y].y, TotalCubeBounds[  Sign.z].z);
	const vec3 CubeMax = vec3(TotalCubeBounds[1-Sign.x].x, TotalCubeBounds[1-Sign.y].y, TotalCubeBounds[1-Sign.z].z);

	int Diepte = 0;

	const vec3 original_tmin = (CubeMin - Begin) * InvRay, original_tmax = (CubeMax - Begin) * InvRay;

	float minimumMaxMin = 0.0f, minmax, maxmin;
	vec3 tmin = original_tmin, tmax = original_tmax, tmid;
	uvec3 Assen;
	uint SubIndex, KindIndex, NodeIndex = 0, PreviousNodeIndex = 0;

	float descends = 1.0f;

	while(Diepte >= 0)
    {

		maxmin = max(minimumMaxMin, max(max(tmin.x, tmin.y), tmin.z));
		minmax = min(min(tmax.x, tmax.y), tmax.z);

		if(maxmin >= minmax || minmax < 0 || maxmin < 0)
			return FaalKleur * descends;

		tmid = (tmin + tmax) * 0.5f;

		for(int as=0; as<3; as++)
			Assen[as] = int(Sign[as] == int(tmid[as] > maxmin));
		    /*if(Sign[as] == 0) //Positief
				Assen[as] = int(tmid[as] < maxmin);
			else
				Assen[as] = int(tmid[as] > maxmin);*/

		SubIndex	= Assen.x + (2 * Assen.y) + (4 * Assen.z);
		KindIndex	= Nodes.data[NodeIndex].Sub[SubIndex];

		if(KindIndex != OCTAL_MAX)
		{
			Diepte++;

			PreviousNodeIndex	= NodeIndex;
			NodeIndex			= KindIndex;

			for(int i=0; i<3; i++)
				if(tmid[i] < maxmin)
					tmin[i] = tmid[i];
				else
					tmax[i] = tmid[i];
		}
		else
		{
			vec4 GevondenNodeKleur =  Nodes.data[PreviousNodeIndex].Kleur;

			if(GevondenNodeKleur.a > 0.5f)
				return GevondenNodeKleur * descends;
			else
			{
				Diepte				= 0;
				NodeIndex			= 0;
				PreviousNodeIndex	= 0;
				minimumMaxMin		= minmax + max(0.0001f, (minmax - maxmin) * 0.001f);
				tmin				= original_tmin;
				tmax				= original_tmax;

				descends -= 0.05f;
			}
		}
    }

	return vec4(1.0f);
}


void main(void)
{
	fColor = GetCubeIntersectColor(Origin, curraydir);

}
