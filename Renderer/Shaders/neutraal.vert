#version 450

in vec4 vertex;
out vec4 color;

void main( void )
{
    gl_Position = vertex;
    color = vertex * 0.5f + vec4(0.5f);
}
