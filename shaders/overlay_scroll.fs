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
uniform bool partGradient;
out vec4 outColor;

void main(){
    if(bottomLeft.x < gl_FragCoord.x && (scrollDimension.x+bottomLeft.x) > gl_FragCoord.x && bottomLeft.y < gl_FragCoord.y && (scrollDimension.y+bottomLeft.y) > gl_FragCoord.y){
        float sizeDiv = horizontal ? scrollDimension.x/float(divCount) : scrollDimension.y/float(divCount);
        float distStart = horizontal ? (gl_FragCoord.x-bottomLeft.x) : (gl_FragCoord.y-bottomLeft.y);
        if(segmentEnabled && (fract(distStart/sizeDiv) < segmentSizeRatio/2 || fract(distStart/sizeDiv) < segmentSizeRatio)){
            outColor = segmentColor;
        }else if( distStart > part*sizeDiv && distStart < (part+1)*sizeDiv ){
            if(partGradient){
                float scrollMask = (distStart - part*sizeDiv)/sizeDiv;
                outColor = scrollMask * partColor + (1-scrollMask) * backgroundColor;
            }else{
                outColor = partColor;
            }
        }else{
            outColor = backgroundColor;
        }
    }
}