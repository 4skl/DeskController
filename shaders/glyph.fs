#version 450 core

in vec2 TexCoords;
out vec4 outColor;

uniform sampler2D text;
uniform vec4 textColor;

void main()
{    
    float sampled = texture(text, TexCoords).r;
    outColor = textColor * sampled;//vec4(textColor.rgb, textColor.a * sampled);
}  