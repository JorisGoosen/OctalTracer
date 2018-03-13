#version 440
#extension GL_ARB_shader_storage_buffer_object :   enable
#extension GL_EXT_gpu_shader4 : enable


out vec4 fColor;
in vec2 TexPos;

uniform mat4 Projectie;
uniform mat4 ModelView;

uniform vec3 Translation;

uniform float fov_y_scale;
uniform float aspect;
uniform vec3 RPos, GPos, BPos;
uniform vec3 RGBFragMultiplier;

#define MAXDIEPTE 32
uniform uint OCTAL_MAX;

struct ShaderOctalNode
{
    vec4 Kleur;
    uint Sub[8];
    //uint Ouder, SubIndex;
    //uint Padding[2];
};

layout(std430, binding = 0) buffer ShaderTree	{ ShaderOctalNode data[]; } Nodes;

vec3 Origin;
vec3 TotalCubeBounds[2] = {vec3(-2.5f), vec3(2.5f)};

struct Stack
{
    uint NodeIndex;
    vec3 tmin, tmax;
};

Stack Pile[MAXDIEPTE];
vec4 Kleuren[8];

// http://chiranjivi.tripod.com/octrav.html

vec4 GetCubeIntersectColor(vec3 Begin, vec3 Ray)
{
    //uint NodeIndex  = 0;
    vec4 FaalKleur  = vec4(Ray * 0.5f + vec3(0.5f), 1.0f);

    vec3 InvRay         = vec3(1.0f) / Ray;
	const ivec3 Sign    = ivec3(int(Ray.x < 0), int(Ray.y < 0), int(Ray.z < 0));
    //vec4 FaalKleur      = vec4(1) - vec4(Sign, 0);

	vec3 CubeMin = vec3(TotalCubeBounds[  Sign.x].x, TotalCubeBounds[  Sign.y].y, TotalCubeBounds[  Sign.z].z);
    vec3 CubeMax = vec3(TotalCubeBounds[1-Sign.x].x, TotalCubeBounds[1-Sign.y].y, TotalCubeBounds[1-Sign.z].z);

    //vec3 CubeMin = TotalCubeBounds[0];
    //vec3 CubeMax = TotalCubeBounds[1];

    int Diepte = 0, SafetyValve = 100;

    Pile[Diepte].NodeIndex	= 0;
    //Pile[Diepte].tmin = min((CubeMin - Begin) * InvRay, (CubeMax - Begin) * InvRay);
    //Pile[Diepte].tmax = max((CubeMin - Begin) * InvRay, (CubeMax - Begin) * InvRay);

    Pile[Diepte].tmin = (CubeMin - Begin) * InvRay;
    Pile[Diepte].tmax = (CubeMax - Begin) * InvRay;

	float minimumMaxMin = 0.0f;

    while(Diepte >= 0 && Diepte < MAXDIEPTE)
    {
        vec3 tmin = Pile[Diepte].tmin;
        vec3 tmax = Pile[Diepte].tmax;

		float maxmin = max(minimumMaxMin, max(max(tmin.x, tmin.y), tmin.z));
        float minmax = min(min(tmax.x, tmax.y), tmax.z);

		if(maxmin >= minmax || minmax < 0 || maxmin < 0)
            return FaalKleur;

        vec3 tmid = (tmin + tmax) * 0.5f;

		uvec3 Assen;

		for(int as=0; as<3; as++)
			if(Sign[as] == 0) //Positief
				Assen[as] = int(tmid[as] < maxmin);
			else
				Assen[as] = int(tmid[as] > maxmin);

		uint SubIndex	= Assen.x + (2 * Assen.y) + (4 * Assen.z);
		uint KindIndex	= Nodes.data[Pile[Diepte].NodeIndex].Sub[SubIndex];

		if(KindIndex != OCTAL_MAX)
		{

			Diepte++;

			if(Diepte >= MAXDIEPTE) return vec4(1,1,0,0);
			//if(Diepte >= 1) return vec4(0,0,1,0);

			Pile[Diepte].NodeIndex = KindIndex;

			for(int i=0; i<3; i++)
			{
				Pile[Diepte].tmin[i] = tmid[i] < maxmin ? tmid[i] : tmin[i];
				Pile[Diepte].tmax[i] = tmid[i] < maxmin ? tmax[i] : tmid[i];
			}
		}
		else
		{
			Diepte--;
			vec4 GevondenNodeKleur =  Nodes.data[Pile[Diepte].NodeIndex].Kleur; //Why Diepte - 1???

			if(GevondenNodeKleur.a > 0.5f) //En ik ben niet doorzichtig dus mijn echte kleur returnen:
				return GevondenNodeKleur;
			else
			{
				//return FaalKleur * 0.5f;
				Diepte = 0;
				minimumMaxMin = minmax + 0.001f;
			}
		}
    }

    if(Diepte >= MAXDIEPTE) return vec4(0, 0, 1, 1);

    return FaalKleur;
}


void main(void)
{
	Kleuren[0] = vec4(0, 0, 0, 1);
	Kleuren[1] = vec4(0, 0, 1, 1);
	Kleuren[2] = vec4(0, 1, 0, 1);
	Kleuren[3] = vec4(1, 0, 0, 1);
	Kleuren[4] = vec4(0, 1, 1, 1);
	Kleuren[5] = vec4(1, 0, 1, 1);
	Kleuren[6] = vec4(1, 1, 0, 1);
	Kleuren[7] = vec4(1, 1, 1, 1);

	//BolPos *= mat3(ModelView);
	Origin = mat3(ModelView) * Translation;//vec4(-Translation, 0.0f) *  ModelView;
	//Origin -= Translation;

	vec3 curraydir =  normalize(mat3(ModelView) * vec3(TexPos.x * fov_y_scale * aspect, TexPos.y * fov_y_scale, 1.0)); //http://blog.hvidtfeldts.net/index.php/2014/01/combining-ray-tracing-and-polygons/

	 //if(curraydir.x < 0 || curraydir.y < 0 || curraydir.z < 0)
	//   fColor = vec4(1);
   // else
	    fColor = GetCubeIntersectColor(Origin, curraydir);

}
