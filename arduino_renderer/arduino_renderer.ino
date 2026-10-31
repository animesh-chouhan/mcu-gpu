#include "config.h"
#include "cube_renderer.h"
#include "framebuffer.h"
#include "led_heart.h"
#include "serial_stream.h"
#include "test_pattern.h"

void setup() {
    beginSerialStream();
    beginLedHeart();
    clearBuffer(0);
}

void loop() {
    updateLedHeart();

    if (USE_SERIAL_TEST_PATTERN) {
        drawTestPattern();
        sendFrame();
        delay(TEST_FRAME_DELAY_MS);
        return;
    }

    updateCubeAnimation();
    drawCubeFrame();
    sendFrame();
    delay(FRAME_DELAY_MS);
}
