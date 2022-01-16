#version 450 core

//in vec2 pos;

uniform vec2 joystickL;
uniform vec2 joystickR;
uniform vec4 backgroundColor;
uniform vec4 foregroundColor;
uniform vec2 mvVector;
out vec4 outColor;

void main(){
   vec4 pos = 0.0052*gl_FragCoord-vec4(1,1,0,0); //todo uniform to get width and height of the window
   float dCenter = sqrt(pos.x*pos.x + pos.y*pos.y);
   if(mvVector.x == 0.5){
      if(dCenter < 0.6){
         outColor = vec4(0.0,0.0,0.0, 0.0);
      }else if(dCenter < 0.8){
         outColor = vec4(foregroundColor.rgb, 0.5);
      }else{
         outColor = vec4(foregroundColor.rgb, 1.0);
      }
      return;
   }

   //draw background first
   float bgMask = max((1-dCenter)-0.4, 0);
   outColor = bgMask * backgroundColor;
   
   //draw first wheel over
   int jPart = int(round((atan(joystickL.y,joystickL.x))/radians(45.0)))%8;
   int pPart = int(round((atan(pos.y,pos.x))/radians(45.0)))%8;
   if((abs(joystickL.x) > 0.05  || abs(joystickL.y) > 0.05)){
      if(jPart == pPart){
         float wheelMask = max((1-dCenter)-0.4, 0);
         outColor = wheelMask * foregroundColor;
      }

      //draw second wheel if fist wheel active
      int count = 32;
      if( fract((atan(pos.y,pos.x)+radians(360.0/float(count*2)))/radians(360.0/float(count))) < 0.05 && dCenter > 0.6 && dCenter < 0.8){//draw borders of first wheel
         outColor = vec4(0.3,0.3,0.3,0.8);
      }else
      if(abs(joystickR.x) > 0.05  || abs(joystickR.y) > 0.05){
         int jRPart = int(round((atan(joystickR.y,joystickR.x))/radians(360.0/float(count))))%count;
         int pRPart = int(round((atan(pos.y,pos.x))/radians(360.0/float(count))))%count;
         if(jRPart == pRPart && dCenter > 0.6 && dCenter < 1){
            float wheel2Mask = max(min(1-dCenter, 1)-0.1, 0);
            outColor = wheel2Mask * vec4(1,0,0,1);
         }
         
      //outColor = sSD(uint(jPart+256*jRPart), vec2(-0.5,0.5), vec2(1,1), vec4(0,1,0.5,1), outColor);
      }
   }

   //outColor = sSD(uint(4096/*+8192+1+2+16+32+128+32768+1024*/), vec2(-1,1), vec2(2,2), vec4(0,1,1,1), outColor);
   /*outColor = sSD(uint(x1), vec2(-1,1), vec2(0.5,0.5), vec4(0,1,1,1), outColor);
   outColor = sSD(uint(x2), vec2(0,1), vec2(0.5,0.5), vec4(0,1,1,1), outColor);
   outColor = sSD(uint(x3), vec2(-1,0), vec2(0.5,0.5), vec4(0,1,1,1), outColor);
   outColor = sSD(uint(x4), vec2(0,0), vec2(0.5,0.5), vec4(0,1,1,1), outColor);*/
   
};
