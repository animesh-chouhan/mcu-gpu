#include "serial_stream.h"

#include "config.h"
#include "framebuffer.h"

static constexpr size_t SERIAL_CHUNK_SIZE = 256;

void beginSerialStream() {
    Serial.begin(BAUD_RATE);
    delay(500);
}

void sendFrame() {
    const uint8_t *data = framebuffer;
    size_t remaining = sizeof(framebuffer);

    Serial.write(START_MARKER);

    while (remaining > 0) {
        const size_t chunkSize = (remaining > SERIAL_CHUNK_SIZE) ? SERIAL_CHUNK_SIZE : remaining;
        Serial.write(data, chunkSize);
        data += chunkSize;
        remaining -= chunkSize;
    }

    Serial.write(END_MARKER);
    Serial.flush();
}
