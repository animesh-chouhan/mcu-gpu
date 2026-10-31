#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "config.h"

extern uint8_t framebuffer[WIDTH * HEIGHT];

void clearBuffer(uint8_t value);
void setPixel(int x, int y, uint8_t color);
void drawLine(int x0, int y0, int x1, int y1, uint8_t color);

#endif
