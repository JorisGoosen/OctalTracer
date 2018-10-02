#version 440

layout(location = 0) in vec4 vertex;

out vec2 TexPos;

void main(void) 
{ 
    gl_Position = vertex;
    TexPos	= vertex.xy;
}


