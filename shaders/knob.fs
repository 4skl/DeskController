#version 450 core

uniform vec4 colorIn;
uniform vec4 colorOut;
uniform vec2 dimension;
out vec4 outColor;

void main(){
    vec2 center = vec2(0, 1);
    vec2 pos = (gl_FragCoord.xy-dimension/2)*2/dimension; // position of the gl_FragCoord in a [-1; 1] square, 0 at center
    float dCenter = sqrt(pow(pos.x-center.x, 2) + pow(pos.y-center.y, 2));
    if(dCenter<1){
        outColor = colorOut*dCenter+colorIn*(1-dCenter);
    }
}