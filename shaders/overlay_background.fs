#version 150 core

in vec2 pos;
uniform vec4 backgroundColor;
uniform vec4 foregroundColor;
out vec4 outColor;

void main(){
   float th = ((pos.y)+1.0)/2.0*4;
   th = sqrt(min(max(th, 0), 1));

   float hR = (-(2/0.7*pos.x+(-2/0.7+1) - pos.y))*2;
   hR = sqrt(min(max(hR, 0), 1));

   float hL = (-(-2/0.7*pos.x+(-2/0.7+1) - pos.y))*2;
   hL = sqrt(min(max(hL, 0), 1));

   /*x : -1 => -0.3
   y : 1 => -1
   D = (y2-y1)/(x2-x1) = (-2)/(-0.7)
   D*x+k = y <=> D*-1+k=1 <=> -D = 1-k <=> -D - 1 = k*/
   float border = th*hL*hR;
   //if(/*-2/0.7*pos.x+(-2/0.7+1) > pos.y - 0.1*/2/0.7*pos.x+(-2/0.7+1) - pos.y > - 0.2){// ||  2/0.7*pos.x+(-2/0.7+1) > pos.y - 0.1){
   /*   float h = (-(2/0.7*pos.x+(-2/0.7+1) - pos.y))*5;
      if(h>1) h = 1;
      if(h<0) h = 0;
      tcolor = h*foregroundColor;
   }*/
   outColor = border*(border * backgroundColor + (1-border) * foregroundColor);
   
};
