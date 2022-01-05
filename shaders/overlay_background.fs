#version 150 core

in vec2 pos;
uniform vec4 color;
out vec4 outColor;

void main(){
   float th = (pos.y)+1.0;
   outColor = th*color;
};