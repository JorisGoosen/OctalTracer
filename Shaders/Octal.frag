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


uniform int OCTAL_MAX;

struct ShaderOctalNode
{
    vec4 Kleur;
    uint Sub[8];
    //uint Ouder;
    //uint Padding[3];
};

layout(std430, binding = 0) buffer ShaderTree	{ ShaderOctalNode data[]; } Nodes;

vec3 Origin;
vec3 TotalCubeBounds[2] = {vec3(-2.5f), vec3(2.5f)};

vec4 GetCubeIntersectColor(vec3 Begin, vec3 Ray)
{
    uint NodeIndex  = 0;
    vec4 FaalKleur  = vec4(Ray * 0.5f + vec3(0.5f), 1.0f);
    vec3 InvRay     = vec3(1.0f) / Ray;
    ivec3 Sign      = ivec3(InvRay.x < 0, InvRay.y < 0, InvRay.z < 0);

    vec3 CubeMin, CubeMax, tmin, tmax;
    float maxmin, minmax;
    vec3 CubeBounds[2] = {TotalCubeBounds[0], TotalCubeBounds[1]};
    vec3 CubePos;
    int Diepte = 0;

    while(true)
    {
        CubeMin = vec3(CubeBounds[Sign.x].x, CubeBounds[Sign.y].y, CubeBounds[Sign.z].z);
        CubeMax = vec3(CubeBounds[1-Sign.x].x, CubeBounds[1-Sign.y].y, CubeBounds[1-Sign.z].z);

        tmin = (CubeMin - Begin) * InvRay;
        tmax = (CubeMax - Begin) * InvRay;

        maxmin = max(max(tmin.x, tmin.y), tmin.z);
        minmax = min(min(tmax.x, tmax.y), tmax.z);

        if(maxmin > minmax)
            return FaalKleur;

       /*if(Nodes.data[NodeIndex].Kleur.a > 0.5f)
            return Nodes.data[NodeIndex].Kleur;
*/
        //Anders dan hebben we misschien kindernodes?
        vec3 CubeExtent = CubeBounds[1] - CubeBounds[0];
        vec3 VageCubePos = ((maxmin * Ray + Begin) - CubeBounds[0]) / CubeExtent;
        CubePos = clamp(round(VageCubePos), 0.0f, 1.0f);

        uint SubIndex = uint(CubePos.x + (2 * CubePos.y) + (4 * CubePos.z));
        uint KindIndex = Nodes.data[NodeIndex].Sub[SubIndex];

        //if(KindIndex == 0) return FaalKleur; //Als de alignment niet goed uitkomt kun je dit krijgen..

        if(KindIndex == OCTAL_MAX)
        {
            if(Nodes.data[NodeIndex].Kleur.a > 0.5f)
                return Nodes.data[NodeIndex].Kleur;

            /*NodeIndex = 0;
            for(int i=0; i<2; i++)
                CubeBounds[i] = TotalCubeBounds[i];

            Begin = Begin + Ray * (minmax + (length(CubeExtent) * 0.1f));

            Diepte = 0;*/
            return vec4(0.0f);
        }
        else
        {

            NodeIndex        = KindIndex;
            CubeExtent      *= 0.5f;
            CubeBounds[0]   += CubeExtent * CubePos;
            CubeBounds[1]    = CubeBounds[0] + CubeExtent;
            Diepte++;
        }
    }
}


void main(void)
{
        //BolPos *= mat3(ModelView);
        Origin = mat3(ModelView) * Translation;//vec4(-Translation, 0.0f) *  ModelView;
        //Origin -= Translation;

        vec3 curraydir =  mat3(ModelView) * normalize(vec3(TexPos.x * fov_y_scale * aspect, TexPos.y * fov_y_scale, 1.0)); //http://blog.hvidtfeldts.net/index.php/2014/01/combining-ray-tracing-and-polygons/

        fColor = GetCubeIntersectColor(Origin, curraydir);

}
