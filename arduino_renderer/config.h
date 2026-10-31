#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

constexpr bool USE_SERIAL_TEST_PATTERN = false;

constexpr int WIDTH = 128;
constexpr int HEIGHT = 96;
constexpr unsigned long BAUD_RATE = 921600;
constexpr uint8_t START_MARKER = 0xAA;
constexpr uint8_t END_MARKER = 0x55;
constexpr unsigned long FRAME_DELAY_MS = 140;
constexpr unsigned long TEST_FRAME_DELAY_MS = 180;
constexpr bool DRAW_AXES_OVERLAY = false;

constexpr float CUBE_SCALE = 18.0f;
constexpr float CAMERA_DISTANCE = 72.0f;
constexpr float PROJECTION_SCALE = 88.0f;
constexpr float ROTATION_STEP_X = 0.04f;
constexpr float ROTATION_STEP_Y = 0.06f;

#endif
