#ifndef DRAW_IMAGE_H
#define DRAW_IMAGE_H
#pragma once
typedef struct {
    int color;
    int length;
} RLEPixel;

void draw_image(int *image, int size_x, int size_y);
void draw_rle_image(RLEPixel *image, int size, int start_x, int start_y);


#endif