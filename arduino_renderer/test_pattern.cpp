#include "test_pattern.h"

#include "config.h"
#include "framebuffer.h"

static uint8_t testFrame = 0;

void drawTestPattern() {
    if ((testFrame & 1) == 0) {
        clearBuffer(0);
        drawLine(0, 0, WIDTH - 1, HEIGHT - 1, 255);
        drawLine(WIDTH - 1, 0, 0, HEIGHT - 1, 255);
    } else {
        clearBuffer(255);
        drawLine(0, HEIGHT / 2, WIDTH - 1, HEIGHT / 2, 0);
        drawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT - 1, 0);
    }

    testFrame++;
}
