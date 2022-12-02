#version 450 core

in vec2 position; // <vec2 pos, vec2 tex>
in vec2 texcoord;
out vec2 TexCoords;

void main()
{
    TexCoords = texcoord;
    gl_Position = vec4(position, 0, 1);
}