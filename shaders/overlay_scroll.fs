#version 450 core

uniform uint part;
uniform vec2 scrollDimension;
uniform uint divCount;
uniform vec2 bottomLeft;
uniform vec4 partColor;
uniform bool horizontal;
uniform bool segmentEnabled;
uniform float segmentSizeRatio = 0.03;
uniform vec4 segmentColor;
uniform vec4 backgroundColor;
out vec4 outColor;

void main(){
    if(bottomLeft.x < gl_FragCoord.x && (scrollDimension.x+bottomLeft.x) > gl_FragCoord.x && bottomLeft.y < gl_FragCoord.y && (scrollDimension.y+bottomLeft.y) > gl_FragCoord.y){
        float sizeDiv = horizontal ? scrollDimension.x/float(divCount+1) : scrollDimension.y/float(divCount+1);
        if(horizontal ? ((gl_FragCoord.x - bottomLeft.x) < float(part+1)*sizeDiv && (gl_FragCoord.x - bottomLeft.x) > float(part)*sizeDiv) : ((gl_FragCoord.y - bottomLeft.y) < float(part+1)*sizeDiv && (gl_FragCoord.y - bottomLeft.y) > float(part)*sizeDiv)){
            float scrollMask = horizontal ? ((gl_FragCoord.x - (bottomLeft.x+sizeDiv*float(part)))/sizeDiv) : ((gl_FragCoord.y - (bottomLeft.y+sizeDiv*float(part)))/sizeDiv);
            outColor = scrollMask * partColor + (1-scrollMask) * backgroundColor;
        }else{
            outColor = backgroundColor;
        }
    }
}