# mcu-gpu

A minimal software-rendering experiment for the Arduino Uno R4 WiFi.

The Arduino builds a grayscale framebuffer in SRAM, streams it over native USB serial, and a small browser UI visualizes the incoming frames on an HTML canvas using the Web Serial API.

## Idea

This project treats a microcontroller like a tiny software GPU pipeline:

1. Render into a fixed framebuffer on the Arduino.
2. Stream the framebuffer to a host computer over USB serial.
3. Decode and display each frame in the browser.

The goal is not high-end graphics performance. The goal is a simple, predictable, readable rendering pipeline that is easy to debug and extend.

## Target Hardware

- Arduino Uno R4 WiFi
- Renesas RA4M1 Cortex-M4 at 48 MHz
- 32 KB SRAM
- Native USB serial connection to the host computer

## Framebuffer Format

The Arduino uses a fixed-size framebuffer stored entirely in SRAM:

```cpp
uint8_t framebuffer[WIDTH * HEIGHT];
```

Parameters:

- Resolution: `128 x 96`
- Pixel format: `8-bit grayscale`
- Framebuffer size: `12288 bytes`

## Serial Protocol

Each frame is sent as a simple packet:

1. Start marker: `0xAA`
2. Raw framebuffer bytes: `WIDTH * HEIGHT`
3. End marker: `0x55`

Packet size:

- `1 + 12288 + 1 = 12290 bytes`

This keeps the transport simple and easy to inspect in the browser debug view.

## What The Arduino Does

The Arduino sketch currently supports two modes.

### 1. Serial Test Mode

Used to validate transport and browser visualization first.

It alternates between very obvious patterns:

- black background with a white `X`
- white background with a black `+`

This mode is controlled by:

```cpp
static const bool USE_SERIAL_TEST_PATTERN = true;
```

### 2. Renderer Mode

When test mode is disabled, the sketch runs the actual framebuffer renderer:

- clears the framebuffer
- updates a small animation state
- draws Bresenham lines into the framebuffer
- streams the result continuously

This gives a minimal software-rendering path that stays readable and deterministic.

## Browser UI

The browser side lives in `ui/` and uses the Web Serial API.

Files:

- `ui/index.html`
- `ui/styles.css`
- `ui/app.js`

The UI:

- connects to the Arduino over serial
- reads incoming byte chunks
- searches for framed packets
- converts grayscale bytes to RGBA for canvas display
- shows raw-byte debug information
- allows connect/disconnect from the device

## Project Layout

```text
.
├── arduino_renderer/
│   └── arduino_renderer.ino
├── ui/
│   ├── app.js
│   ├── index.html
│   └── styles.css
├── .vscode/
│   └── tasks.json
└── viewer.html
```

Notes:

- `viewer.html` is the older single-file viewer.
- The split UI in `ui/` is the preferred version.

## Uploading The Sketch

First, make sure `arduino-cli` is installed and the Uno R4 core is available.

Install core support:

```bash
arduino-cli core update-index
arduino-cli core install arduino:renesas_uno
```

Compile:

```bash
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi /home/animesh/Github/mcu-gpu/arduino_renderer
```

Upload:

```bash
arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:renesas_uno:unor4wifi /home/animesh/Github/mcu-gpu/arduino_renderer
```

If upload fails because the port is busy:

- close the browser serial connection
- close any serial monitor
- reconnect the board
- run `arduino-cli board list` to confirm the port

## Running The Browser UI

Serve the project locally:

```bash
cd /home/animesh/Github/mcu-gpu
python3 -m http.server 8000
```

Open:

```text
http://localhost:8000/ui/
```

Then:

1. Upload the Arduino sketch.
2. Open the browser UI.
3. Click `Connect Serial`.
4. Choose the Arduino serial device.

## Debugging Notes

The UI includes a raw-byte debug panel that shows:

- last chunk size
- total bytes received
- buffered bytes
- frame counters
- start/end marker positions
- hex preview of recent bytes

This helps separate transport problems from rendering problems.

Typical status messages:

- `Disconnected`
- `Connected, waiting for frames`
- `Receiving bytes: ...`
- `Previewing payload only: ...`
- `Streaming framed packets: ...`

## Why This Project Exists

This project is a compact testbed for:

- software rendering on constrained hardware
- framebuffer-based graphics pipelines
- serial frame transport
- browser-side visualization tools
- debugging graphics systems incrementally

It is intentionally minimal so we can reason about every step of the pipeline.

## Next Steps

Natural directions to extend this project:

1. Switch back to higher baud rates after validating transport reliability.
2. Add more primitives such as rectangles, circles, or triangle filling.
3. Add timing and FPS counters on both the Arduino and browser side.
4. Add a richer renderer such as a wireframe cube or tile-based rasterizer.
5. Replace the debug protocol with a more robust framed transport if needed.
