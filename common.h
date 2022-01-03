#ifndef HEADER_COMMON
#define HEADER_COMMON

struct ColorRGBf {
    float r;
    float g;
    float b;
};
typedef struct ColorRGBf ColorRGBf;

enum Side {TOP, BOTTOM, LEFT, RIGHT};
typedef enum Side Side;

#endif