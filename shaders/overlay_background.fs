#version 150 core

in vec2 pos;
uniform vec2 joystickL;
uniform vec2 joystickR;
uniform vec4 backgroundColor;
uniform vec4 foregroundColor;
out vec4 outColor;

void main(){
   int jPart = int(round((atan(joystickL.y,joystickL.x))/radians(45.0)))%8;
   int pPart = int(round((atan(pos.y,pos.x))/radians(45.0)))%8;
   if((abs(joystickL.x) > 0.05  || abs(joystickL.y) > 0.05) && jPart == pPart){
      float border = 1-max(sqrt(pos.x*pos.x + pos.y*pos.y)-0.1, 0);
      outColor = border * foregroundColor;
   }else{
      float border = 1-sqrt(pos.x*pos.x + pos.y*pos.y);
      outColor = border * backgroundColor;
   }
   
   
};
