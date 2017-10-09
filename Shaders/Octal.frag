#version 440
#extension GL_ARB_compute_shader : enable
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


//Perlin Stuff:

uniform int OCTAL_MAX;

struct ShaderOctalNode
{
    vec4 Kleur;
    uint Sub[8];
};

layout(std430, binding = 0) buffer ShaderTree	{ ShaderOctalNode data[]; } Nodes;

vec3 Origin, BolPos = vec3(0.0f, 0.0f, 0.0f);
float BolRadius = 2.0f, TresholdDist = 0.001f;// Good for rendering sphere only: 0.000001f;
const float MarchDistAttenuate = 1.0f;

float GetMarchingDistance(vec3 Pos)
{
/*	vec4 BolRelativePos = Pos - BolPos;

	float Bol = max(0.0f, length(BolRelativePos) - BolRadius;// + (GetIniqoQuilesNoise(Angles) * 0.025f)  );//+ (0.025f * sin(Pos.x * 6.28 * 12)) + (0.045f * sin(Pos.y * 6.28 * 4)) + (0.015f * sin(Pos.z * 6.28 * 6)) );

	return Bol;
*/
		//(0.025f * sin(Pos.x * 6.28 * 12)) + (0.045f * sin(Pos.y * 6.28 * 4)) + (0.035f * sin(Pos.z * 6.28 * 6)) );

	return max(0.0f, length(Pos - BolPos) - BolRadius);
}


vec3 GetNormal(vec3 Pos)
{
        const float Offset = 0.001f;

	return normalize(
                vec3(
                        GetMarchingDistance(Pos + vec3(Offset, 0, 0)) - GetMarchingDistance(Pos - vec3(Offset, 0, 0)),
                        GetMarchingDistance(Pos + vec3(0, Offset, 0)) - GetMarchingDistance(Pos - vec3(0, Offset, 0)),
                        GetMarchingDistance(Pos + vec3(0, 0, Offset)) - GetMarchingDistance(Pos - vec3(0, 0, Offset))
		));
}


const float StepSize = 0.01f, DensityNoiseStop = 0.25f;

vec3 GetInigoNormal(vec3 Pos)
{
        const float Offset = 0.1f;
	return normalize(
                vec3(
                        GetIniqoQuilesNoise(Pos + vec3(Offset, 0, 0)) - GetIniqoQuilesNoise(Pos - vec3(Offset, 0, 0)),
                        GetIniqoQuilesNoise(Pos + vec3(0, Offset, 0)) - GetIniqoQuilesNoise(Pos - vec3(0, Offset, 0)),
                        GetIniqoQuilesNoise(Pos + vec3(0, 0, Offset)) - GetIniqoQuilesNoise(Pos - vec3(0, 0, Offset))
		));
}

vec3 LastFoundPos;

//Returns Last Moved Distance at the time it doesnt feel like going on anymore, if < 0 this means nothing found else distance to closest object this means it found the object
float MarchToObject(vec3 StartPos, vec3 Ray)
{
	int step = 0;
        float TotalDist = 0.0f, LastMove = 0.0f;
	
	while(TotalDist < 100.0f)
	{
                LastFoundPos = StartPos + TotalDist * Ray;
		//float MarchDist = GetMarchingDistance(LastFoundPos + (0.01f * PerlinGradients.data_Gradients[(step++)%PerlinSize]));
		float MarchDist = GetMarchingDistance(LastFoundPos);
		
		if(MarchDist < TresholdDist)
		{
                        if(GetIniqoQuilesNoise(LastFoundPos) < DensityNoiseStop)
                            MarchDist = StepSize;
                        else
                            return LastMove;


                    //return MarchDist;
		}

                LastMove = MarchDist;
		TotalDist += MarchDist * MarchDistAttenuate;
	}

	return -1;
}


float CalculateShadow(vec3 LightPos, vec3 ObjectPos, float K)
{

	//Rustig over nadenken ofzo?

        vec3 Ray = normalize(LightPos - ObjectPos);
        vec3 StartPos  = ObjectPos;

	float TotalDist = 0.0f;
        //float Licht = 1.0f;

	while(TotalDist < 100.0f)
	{
		LastFoundPos = StartPos + TotalDist * Ray;

		float MarchDist = GetMarchingDistance(LastFoundPos);

                if(MarchDist < TresholdDist)
		{
                   if(GetIniqoQuilesNoise(LastFoundPos) < DensityNoiseStop)
                        MarchDist = StepSize;
                    else
                        return 0.0f;
                    //return 1.0f - Licht;
		}

                //Licht = min(Licht, K * MarchDist / TresholdDist);

		TotalDist += MarchDist * MarchDistAttenuate;
	}

        return  dot(Ray, GetInigoNormal(StartPos)); //GetInigoNormal(ObjectPos));
        //return Licht;
}

vec4 MarchToColor(vec3 Ray)
{
	float MarchDist = MarchToObject(Origin, Ray);
	
	if(MarchDist < 0)
                return vec4(Ray * 0.5f + vec3(0.5f), 0.0f);//vec4(0.0f); //We zijn te ver weg gegaan!

	//Anders hebben we het object gevonden. Nu nog de licht-bijdrage!

       // return vec4(1.0f);

      //  vec3 IntersectPos = LastFoundPos;
        vec3 SafeObjectPos = LastFoundPos - (MarchDist * Ray);

        float RLightContrib = CalculateShadow(RPos, SafeObjectPos, 1.0f);
        //float GLightContrib = CalculateShadow(GPos, IntersectPos, 1.0f);
        //float BLightContrib = CalculateShadow(BPos, IntersectPos, 1.0f);
	
        return vec4(RLightContrib);//, GLightContrib, BLightContrib, 256.0f);

        //return vec4(dot(normalize(RPos.xyz - IntersectPos), GetNormal(IntersectPos)));

        //return vec4(GetNormal(IntersectPos) * 0.5f + vec3(0.5f), 0.0f);
        //return vec4(1.0f);
}



void main(void)
{
        //BolPos *= mat3(ModelView);
        Origin = vec3(0.0f, 0.0f, 10.0f) * mat3(ModelView);//vec4(-Translation, 0.0f) *  ModelView;
        Origin -= Translation;

        vec3 curraydir = normalize(vec3(TexPos.x * fov_y_scale * aspect, TexPos.y * fov_y_scale, -1.0) * mat3(ModelView)); //http://blog.hvidtfeldts.net/index.php/2014/01/combining-ray-tracing-and-polygons/

	fColor = MarchToColor(curraydir);

	//fColor = vec4(GetIniqoQuilesNoise(vec3(TexPos * 0.5f + vec2(0.5f), 0)));//, GetPerlinNoise(vec3(TexPos* 0.5f + vec2(0.5f), 0.5f)), GetPerlinNoise(vec3(TexPos* 0.5f + vec2(0.5f), 1)), 1);
	
}
