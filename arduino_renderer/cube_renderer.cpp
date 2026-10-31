#include "cube_renderer.h"

#include <math.h>

#include "config.h"
#include "framebuffer.h"

struct Vec3 {
    float x;
    float y;
    float z;
};

struct ScreenPoint {
    int x;
    int y;
    bool visible;
};

struct Edge {
    uint8_t a;
    uint8_t b;
};

static float cubeAngleX = 0.0f;
static float cubeAngleY = 0.0f;
static float cubeSinX = 0.0f;
static float cubeCosX = 1.0f;
static float cubeSinY = 0.0f;
static float cubeCosY = 1.0f;

static const Vec3 kCubeVertices[8] = {
    {-1.0f, -1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f},
    { 1.0f,  1.0f, -1.0f},
    {-1.0f,  1.0f, -1.0f},
    {-1.0f, -1.0f,  1.0f},
    { 1.0f, -1.0f,  1.0f},
    { 1.0f,  1.0f,  1.0f},
    {-1.0f,  1.0f,  1.0f},
};

static const Edge kCubeEdges[12] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    {0, 4}, {1, 5}, {2, 6}, {3, 7},
};

static Vec3 rotateVertex(const Vec3 &vertex) {
    const float rotatedX = (vertex.x * cubeCosY) + (vertex.z * cubeSinY);
    const float rotatedZ = (-vertex.x * cubeSinY) + (vertex.z * cubeCosY);

    Vec3 result;
    result.x = rotatedX;
    result.y = (vertex.y * cubeCosX) - (rotatedZ * cubeSinX);
    result.z = (vertex.y * cubeSinX) + (rotatedZ * cubeCosX);
    return result;
}

static ScreenPoint projectVertex(const Vec3 &vertex) {
    ScreenPoint point;
    const float depth = vertex.z + CAMERA_DISTANCE;

    if (depth <= 0.1f) {
        point.x = 0;
        point.y = 0;
        point.visible = false;
        return point;
    }

    const float scale = PROJECTION_SCALE / depth;
    point.x = static_cast<int>((vertex.x * scale) + (WIDTH * 0.5f));
    point.y = static_cast<int>((vertex.y * scale) + (HEIGHT * 0.5f));
    point.visible = true;
    return point;
}

static void drawAxesOverlay() {
    const int centerX = WIDTH / 2;
    const int centerY = HEIGHT / 2;
    drawLine(centerX, 0, centerX, HEIGHT - 1, 32);
    drawLine(0, centerY, WIDTH - 1, centerY, 32);
}

void updateCubeAnimation() {
    cubeAngleX += ROTATION_STEP_X;
    cubeAngleY += ROTATION_STEP_Y;

    if (cubeAngleX > TWO_PI) {
        cubeAngleX -= TWO_PI;
    }

    if (cubeAngleY > TWO_PI) {
        cubeAngleY -= TWO_PI;
    }

    cubeSinX = sinf(cubeAngleX);
    cubeCosX = cosf(cubeAngleX);
    cubeSinY = sinf(cubeAngleY);
    cubeCosY = cosf(cubeAngleY);
}

void drawCubeFrame() {
    ScreenPoint projected[8];

    clearBuffer(0);
    if (DRAW_AXES_OVERLAY) {
        drawAxesOverlay();
    }

    for (int i = 0; i < 8; i++) {
        Vec3 transformed = rotateVertex(kCubeVertices[i]);
        transformed.x *= CUBE_SCALE;
        transformed.y *= CUBE_SCALE;
        transformed.z *= CUBE_SCALE;
        projected[i] = projectVertex(transformed);
    }

    for (int i = 0; i < 12; i++) {
        const ScreenPoint &a = projected[kCubeEdges[i].a];
        const ScreenPoint &b = projected[kCubeEdges[i].b];

        if (!a.visible || !b.visible) {
            continue;
        }

        drawLine(a.x, a.y, b.x, b.y, 255);
    }
}
