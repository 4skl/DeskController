#version 450 core

uniform uint part;
uniform vec2 wheelDimension;
uniform uint divCount;
uniform vec2 center;
uniform vec2 circleMinMax;
uniform vec4 partColor;
uniform bool segmentEnabled;
uniform float segmentAngleRatio = 0.03;
uniform vec4 segmentColor;
uniform vec4 backgroundColor;
uniform bool partGradient;
out vec4 outColor;

const float PI = 3.141592653589793;

void main(){
    if(gl_FragCoord.x > center.x-wheelDimension.x/2 && gl_FragCoord.x < center.x+wheelDimension.x/2 && gl_FragCoord.y > center.y-wheelDimension.y/2 && gl_FragCoord.y < center.y+wheelDimension.y/2){
        float sizeDiv = 2*PI/float(divCount);
        //why *2 ?
        vec2 pos = (gl_FragCoord.xy-center)*2/wheelDimension; // position of the gl_FragCoord in a [-1; 1] square, 0 at center
        float dCenter = sqrt(pos.x*pos.x + pos.y*pos.y);

        if(dCenter > circleMinMax.x && dCenter < circleMinMax.y){
            float posAngle = atan(-pos.y,pos.x) + PI;
            float partF = posAngle / sizeDiv;
            if(segmentEnabled && (fract(partF) < segmentAngleRatio/2 || 1-fract(partF) < segmentAngleRatio/2)){//draw borders of wheel // todo correct
                outColor = segmentColor;
            }else if(part == uint(partF)){
                if(partGradient){
                    float wheelMask = (dCenter - circleMinMax.x)/(circleMinMax.y-circleMinMax.x);
                    outColor = wheelMask * partColor + (1-wheelMask) * backgroundColor;
                }else{
                    outColor = partColor;
                }
                
            }else{
                outColor = backgroundColor;
            }
        }
    }
}