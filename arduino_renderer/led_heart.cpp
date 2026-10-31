#include "led_heart.h"

#include <Arduino_LED_Matrix.h>

ArduinoLEDMatrix matrix;

static uint8_t HEART_FRAME_1[8][12] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

static uint8_t HEART_FRAME_2[8][12] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

struct HeartFrame {
    uint8_t (*bitmap)[12];
    unsigned long durationMs;
};

static const HeartFrame HEART_SEQUENCE[] = {
    {HEART_FRAME_1, 66},
    {HEART_FRAME_2, 66},
};

static size_t currentHeartFrame = 0;
static unsigned long lastHeartFrameMs = 0;

void beginLedHeart() {
    matrix.begin();
    matrix.clear();
    matrix.renderBitmap(HEART_SEQUENCE[0].bitmap, 8, 12);
    lastHeartFrameMs = millis();
}

void updateLedHeart() {
    const unsigned long now = millis();
    const unsigned long durationMs = HEART_SEQUENCE[currentHeartFrame].durationMs;

    if (now - lastHeartFrameMs < durationMs) {
        return;
    }

    currentHeartFrame = (currentHeartFrame + 1) % (sizeof(HEART_SEQUENCE) / sizeof(HEART_SEQUENCE[0]));
    matrix.renderBitmap(HEART_SEQUENCE[currentHeartFrame].bitmap, 8, 12);
    lastHeartFrameMs = now;
}
