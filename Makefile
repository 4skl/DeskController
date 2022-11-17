CC=gcc
CFLAGS=`pkg-config --cflags freetype2` -I.
INPUT_C=main.c overlay.c load_shader.c common.c
OUTPUT=DeskController.exe
LIBS=-lfreetype -lpng -lz -lglew32 -lglfw3 -lgdi32 -lopengl32

main:
	$(CC) $(CFLAGS) -o $(OUTPUT) $(INPUT_C) $(LIBS)
