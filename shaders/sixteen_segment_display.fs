// --(0)-- --(1)--    = size y/14
// |\     |      /|
// | \    |     / |
//(7)(A) (B) (C) (2)
// |   \  |  /    |
// |    \ | /     |
// --(8)-- --(9)--
// |    / | \     |
// |   /  |  \    |
//(6)(D) (E) (F) (3)
// | /    |     \ |
// |/     |      \|
// --(5)-- --(4)--

#define A 975
#define B 37363
#define C 243

#define x1 18481
#define x2 12467
#define x3 831
#define x4 908

//    1
// -1 0 1
//   -1


vec4 sSD(uint letterCode, vec2 position, vec2 size, vec4 drawColor, vec4 bgColor){ //letterCode is a 16bit code for display (0 = off, 1 = on)
   float wFact = 14.0;
   /* draw horizontal lines */
   if(bool(letterCode & uint(1))){ //draw line 0
      if(pos.x > position.x && pos.x < position.x+size.x/2.0 \
         && pos.y < position.y && pos.y > position.y-size.y/wFact) return drawColor;
   }
   if(bool(letterCode & uint(2))){ //draw line 1
      if(pos.x > position.x+size.x/2.0 && pos.x < position.x+size.x \
         && pos.y < position.y && pos.y > position.y-size.y/wFact) return drawColor;
   }

   if(bool(letterCode & uint(256))){ //draw line 8
      if(pos.x > position.x && pos.x < position.x+size.x/2.0 \
         && pos.y < position.y-size.y/2.0+(size.y/wFact/2.0) && pos.y > position.y-size.y/2.0-(size.y/wFact/2.0)) return drawColor;
   }
   if(bool(letterCode & uint(512))){ //draw line 9
      if(pos.x > position.x+size.x/2.0 && pos.x < position.x+size.x \
         && pos.y < position.y-size.y/2.0+(size.y/wFact/2.0) && pos.y > position.y-size.y/2.0-(size.y/wFact/2.0)) return drawColor;
   }

   if(bool(letterCode & uint(32))){ //draw line 5
      if(pos.x > position.x && pos.x < position.x+size.x/2.0 \
         && pos.y < position.y-size.y+size.y/14.0 && pos.y > position.y-size.y) return drawColor;
   }
   if(bool(letterCode & uint(16))){ //draw line 4
      if(pos.x > position.x+size.x/2.0 && pos.x < position.x+size.x \
         && pos.y < position.y-size.y+size.y/wFact && pos.y > position.y-size.y) return drawColor;
   }

   /* draw vertical lines */
   if(bool(letterCode & uint(4))){ //draw line 2
      if(pos.x > position.x+size.x-size.x/wFact && pos.x < position.x+size.x \
         && pos.y < position.y && pos.y > position.y-size.y/2.0) return drawColor;
   }
   if(bool(letterCode & uint(8))){ //draw line 3
      if(pos.x > position.x+size.x-size.x/wFact && pos.x < position.x+size.x \
         && pos.y < position.y-size.y/2.0 && pos.y > position.y-size.y+size.y/wFact) return drawColor;
   }

   if(bool(letterCode & uint(128))){ //draw line 7
      if(pos.x > position.x && pos.x < position.x+size.x/wFact \
         && pos.y < position.y && pos.y > position.y-size.y/2.0) return drawColor;
   }
   if(bool(letterCode & uint(64))){ //draw line 6
      if(pos.x > position.x && pos.x < position.x+size.x/wFact  \
         && pos.y < position.y-size.y/2.0 && pos.y > position.y-size.y+size.y/wFact) return drawColor;
   }

   if(bool(letterCode & uint(2048))){ //draw line B
      if(pos.x > position.x+size.x/2.0-(size.x/wFact/2.0) && pos.x < position.x+size.x/2.0+(size.x/wFact/2.0) \
         && pos.y < position.y && pos.y > position.y-size.y/2.0) return drawColor;
   }
   if(bool(letterCode & uint(16384))){ //draw line E
      if(pos.x > position.x+size.x/2.0-(size.x/wFact/2.0) && pos.x < position.x+size.x/2.0+(size.x/wFact/2.0)  \
         && pos.y < position.y-size.y/2.0 && pos.y > position.y-size.y+size.y/wFact) return drawColor;
   }

   /* draw diagonal lines */
   if(bool(letterCode & uint(1024))){ //draw line A
      if(abs(-(pos.x-position.x)/size.x - (pos.y-position.y)/size.y) < 1/wFact && 
      pos.y < position.y && pos.y > position.y-size.y/2.0 && pos.x > position.x && pos.x < position.x+size.x/2) return drawColor;
   }
   if(bool(letterCode & uint(32768))){ //draw line F
      if(abs(-(pos.x-position.x)/size.x - (pos.y-position.y)/size.y) < 1.0/wFact && 
      pos.y < position.y-size.y/2.0 && pos.y > position.y-size.y && pos.x > position.x+size.x/2 && pos.x < position.x+size.x) return drawColor;
   }
   if(bool(letterCode & uint(4096))){ //draw line C
      if(abs((pos.x-position.x)/size.x - (pos.y-position.y)/size.y) < 1.0/wFact && 
      pos.y < position.y && pos.y > position.y-size.y/2.0 && pos.x > position.x+size.x/2.0 && pos.x < position.x+size.x) return drawColor;
   }
   if(bool(letterCode & uint(8192))){ //draw line D
      if(abs((pos.x-position.x)/size.x - (pos.y-position.y)/size.y) < 1.0/wFact && 
      pos.y < position.y-size.y/2.0 && pos.y > position.y-size.y && pos.x > position.x && pos.x < position.x+size.x/2.0) return drawColor;
   }
   return bgColor;
}