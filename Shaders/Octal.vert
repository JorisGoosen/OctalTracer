#version 440

layout(location = 0) in vec4 vertex;


uniform mat4 Projectie;
uniform mat4 ModelView;

uniform vec3 Translation;

uniform float fov_y_scale;
uniform float aspect;

out vec3 Origin;
out vec3 curraydir;

void main(void) 
{ 
    gl_Position = vertex;
	vec2 TexPos	= vertex.xy;

	Origin = mat3(ModelView) * Translation;//vec4(-Translation, 0.0f) *  ModelView;
	curraydir =  normalize(mat3(ModelView) * vec3(TexPos.x * fov_y_scale * aspect, TexPos.y * fov_y_scale, 1.0)); //http://blog.hvidtfeldts.net/index.php/2014/01/combining-ray-tracing-and-polygons/

}


