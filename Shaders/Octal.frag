#version 440
//#extension GL_ARB_compute_shader : enable
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

#define MAXDIEPTE 20
uniform int OCTAL_MAX;

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

// http://chiranjivi.tripod.com/octrav.html

vec4 GetCubeIntersectColor(vec3 Begin, vec3 Ray)
{
    //uint NodeIndex  = 0;
    //vec4 FaalKleur  = vec4(Ray * 0.5f + vec3(0.5f), 1.0f);

    vec3 InvRay         = vec3(1.0f) / Ray;
    const ivec3 Sign    = ivec3(Ray.x < 0, Ray.y < 0, Ray.z < 0);
    vec4 FaalKleur      = vec4(1) - vec4(Sign, 0);

    vec3 CubeMin = vec3(TotalCubeBounds[  Sign.x].x, TotalCubeBounds[  Sign.y].y, TotalCubeBounds[  Sign.z].z);
    vec3 CubeMax = vec3(TotalCubeBounds[1-Sign.x].x, TotalCubeBounds[1-Sign.y].y, TotalCubeBounds[1-Sign.z].z);

    int Diepte = 0, SafetyValve = 100;

    Pile[Diepte].NodeIndex	= 0;
    Pile[Diepte].tmin = (CubeMin - Begin) * InvRay;
    Pile[Diepte].tmax = (CubeMax - Begin) * InvRay;

    while(Diepte >= 0 && Diepte < MAXDIEPTE)
    {
        /*vec3 tmin = min(Pile[Diepte].tmin, Pile[Diepte].tmax);
        vec3 tmax = max(Pile[Diepte].tmin, Pile[Diepte].tmax);*/

        vec3 tmin = Pile[Diepte].tmin;
        vec3 tmax = Pile[Diepte].tmax;

        float maxmin = max(max(tmin.x, tmin.y), tmin.z);
        float minmax = min(min(tmax.x, tmax.y), tmax.z);

	if(maxmin > minmax || (minmax < 0 && maxmin < 0))
            return FaalKleur;

        vec3 tmid = (tmin + tmax) * 0.5f;

        ivec3 tpos = ivec3(int(tmid.x < minmax), int(tmid.y < minmax), int(tmid.z < minmax));

       // for(int i=0; i<3; i++)
         //   if(Sign[i] == 1) tpos[i] = 1 - tpos[i];


	int SubIndex = tpos.x + (2 * tpos.y) + (4 * tpos.z);

	uint KindIndex	= Nodes.data[Pile[Diepte].NodeIndex].Sub[SubIndex];

	//return Nodes.data[Pile[Diepte].NodeIndex].Kleur;;

	if(KindIndex != OCTAL_MAX)
	{
	    Diepte++;

	    if(Diepte >= MAXDIEPTE) return vec4(0,0,0,0);

	    Pile[Diepte].NodeIndex = KindIndex;

	    for(int i=0; i<3; i++)
	    {
		if(tpos[i] == 0)
		{
		    Pile[Diepte].tmin[i] = Pile[Diepte - 1].tmin[i];
		    Pile[Diepte].tmax[i] = tmid[i];
		}
		else
		{
		    Pile[Diepte].tmin[i] = tmid[i];
		    Pile[Diepte].tmax[i] = Pile[Diepte - 1].tmax[i];
		}
	    }
	}
	else
        {
	    //Kinderen zijn er niet. Dus kijken of we in een doorzichtige cel zitten of niet.

	    //if(Nodes.data[Pile[Diepte].NodeIndex].Kleur.a > 0.5f) //En ik ben niet doorzichtig dus mijn echte kleur returnen:
		return Nodes.data[Pile[Diepte].NodeIndex].Kleur;
	        //return vec4(1 - (Diepte / MAXDIEPTE));

	}

    }

    if(Diepte >= MAXDIEPTE) return vec4(0, 0, 1, 1);

    return FaalKleur;
}


void main(void)
{
        //BolPos *= mat3(ModelView);
        Origin = mat3(ModelView) * Translation;//vec4(-Translation, 0.0f) *  ModelView;
        //Origin -= Translation;

        vec3 curraydir =  mat3(ModelView) * normalize(vec3(TexPos.x * fov_y_scale * aspect, TexPos.y * fov_y_scale, 1.0)); //http://blog.hvidtfeldts.net/index.php/2014/01/combining-ray-tracing-and-polygons/

         //if(curraydir.x < 0 || curraydir.y < 0 || curraydir.z < 0)
        //   fColor = vec4(1);
       // else
            fColor = GetCubeIntersectColor(Origin, curraydir);

}
