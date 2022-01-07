#version 150 core

in vec2 pos;
uniform vec2 joystickL;
uniform vec2 joystickR;
uniform vec4 backgroundColor;
uniform vec4 foregroundColor;
out vec4 outColor;

void main(){
   float dCenter = sqrt(pos.x*pos.x + pos.y*pos.y);

   //draw background first
   float bgMask = (1-dCenter*0.5);
   outColor = bgMask * backgroundColor;
   
   //draw first wheel over
   int jPart = int(round((atan(joystickL.y,joystickL.x))/radians(45.0)))%8;
   int pPart = int(round((atan(pos.y,pos.x))/radians(45.0)))%8;
   if((abs(joystickL.x) > 0.05  || abs(joystickL.y) > 0.05)){
      if(jPart == pPart){
         float wheelMask = (1-max(dCenter-0.1, 0))*0.8;
         outColor = wheelMask * foregroundColor;
      }

      //draw second wheel if fist wheel active
      if(abs(joystickR.x) > 0.05  || abs(joystickR.y) > 0.05){
         int count = 16;
         int jRPart = int(round((atan(joystickR.y,joystickR.x))/radians(360.0/float(count))))%count;
         int pRPart = int(round((atan(pos.y,pos.x))/radians(360.0/float(count))))%count;
         if(jRPart == pRPart && dCenter > 0.6){
            float wheel2Mask = min(dCenter+0.6, 1);
            outColor = wheel2Mask * foregroundColor;
         }
      }
   }  
   
};
