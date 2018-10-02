#version 450

in vec4 color;
layout(location = 0, index = 0) out vec4 fragColor;

void main( void )
{
    fragColor = color;
}
