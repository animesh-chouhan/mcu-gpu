const WIDTH = 128;
const HEIGHT = 96;
const FRAME_SIZE = WIDTH * HEIGHT;
const START_MARKER = 0xAA;
const END_MARKER = 0x55;
const BAUD_RATE = 921600;

const connectButton = document.getElementById("connectButton");
const disconnectButton = document.getElementById("disconnectButton");
const statusLabel = document.getElementById("status");
const rawBytesLabel = document.getElementById("rawBytes");
const canvas = document.getElementById("screen");
const ctx = canvas.getContext("2d", { alpha: false });
const imageData = ctx.createImageData(WIDTH, HEIGHT);

let port = null;
let reader = null;
let readBuffer = new Uint8Array(0);
let keepReading = false;
let bytesReceived = 0;
let framesRendered = 0;
let previewFramesRendered = 0;

function setStatus(text) {
  statusLabel.textContent = text;
}

function setConnectionUi(connected) {
  connectButton.disabled = connected;
  disconnectButton.disabled = !connected;
}

function updateRawBytesView(chunk) {
  const previewLength = Math.min(chunk.length, 64);
  const preview = [];
  const startIndex = readBuffer.indexOf(START_MARKER);
  const endIndex = readBuffer.indexOf(END_MARKER, startIndex >= 0 ? startIndex + 1 : 0);

  for (let i = 0; i < previewLength; i++) {
    preview.push(chunk[i].toString(16).padStart(2, "0"));
  }

  rawBytesLabel.textContent =
    `Last chunk size: ${chunk.length}\n` +
    `Total bytes received: ${bytesReceived}\n` +
    `Buffered bytes: ${readBuffer.length}\n` +
    `Frames rendered: ${framesRendered}\n` +
    `Preview frames rendered: ${previewFramesRendered}\n` +
    `Start marker index: ${startIndex}\n` +
    `End marker index: ${endIndex}\n` +
    `Preview (${previewLength} bytes): ${preview.join(" ")}`;
}

function appendChunk(existing, chunk) {
  const merged = new Uint8Array(existing.length + chunk.length);
  merged.set(existing, 0);
  merged.set(chunk, existing.length);
  return merged;
}

function renderFrame(frame) {
  const rgba = imageData.data;
  let rgbaIndex = 0;

  for (let i = 0; i < frame.length; i++) {
    const gray = frame[i];
    rgba[rgbaIndex++] = 0;
    rgba[rgbaIndex++] = gray;
    rgba[rgbaIndex++] = 32;
    rgba[rgbaIndex++] = 255;
  }

  ctx.putImageData(imageData, 0, 0);
}

function findFramedPacket() {
  const packetSize = FRAME_SIZE + 2;

  if (readBuffer.length < packetSize) {
    return null;
  }

  for (let startIndex = 0; startIndex <= readBuffer.length - packetSize; startIndex++) {
    if (readBuffer[startIndex] !== START_MARKER) {
      continue;
    }

    const frameStart = startIndex + 1;
    const frameEnd = frameStart + FRAME_SIZE;

    if (readBuffer[frameEnd] === END_MARKER) {
      return { startIndex, frameStart, frameEnd };
    }
  }

  return null;
}

function extractFrame() {
  const packet = findFramedPacket();

  if (!packet) {
    if (readBuffer.length > FRAME_SIZE + 2) {
      readBuffer = readBuffer.slice(readBuffer.length - (FRAME_SIZE + 2));
    }
    return null;
  }

  const frame = readBuffer.slice(packet.frameStart, packet.frameEnd);
  readBuffer = readBuffer.slice(packet.frameEnd + 1);
  return frame;
}

function extractPreviewFrame() {
  const startIndex = readBuffer.indexOf(START_MARKER);

  if (startIndex < 0) {
    return null;
  }

  const frameStart = startIndex + 1;
  const frameEnd = frameStart + FRAME_SIZE;

  if (readBuffer.length < frameEnd) {
    return null;
  }

  return readBuffer.slice(frameStart, frameEnd);
}

async function readLoop() {
  while (keepReading && port && port.readable) {
    reader = port.readable.getReader();

    try {
      while (true) {
        const { value, done } = await reader.read();

        if (done) {
          break;
        }

        if (!value || value.length === 0) {
          continue;
        }

        bytesReceived += value.length;
        readBuffer = appendChunk(readBuffer, value);
        updateRawBytesView(value);

        if (bytesReceived > 0 && readBuffer.length < FRAME_SIZE + 2) {
          setStatus(`Receiving bytes: ${bytesReceived}`);
        }

        let frame = extractFrame();
        while (frame !== null) {
          renderFrame(frame);
          framesRendered += 1;
          setStatus(`Streaming framed packets: ${framesRendered}`);
          frame = extractFrame();
        }

        if (framesRendered === 0) {
          const previewFrame = extractPreviewFrame();
          if (previewFrame !== null) {
            renderFrame(previewFrame);
            previewFramesRendered += 1;
            setStatus(`Previewing payload only: ${previewFramesRendered}`);
          }
        }
      }
    } catch (error) {
      if (keepReading) {
        throw error;
      }
    } finally {
      reader.releaseLock();
      reader = null;
    }
  }
}

async function disconnectSerial() {
  keepReading = false;

  try {
    if (reader) {
      await reader.cancel();
    }
  } catch (error) {
    console.error(error);
  }

  try {
    if (port) {
      await port.close();
    }
  } catch (error) {
    console.error(error);
  }

  port = null;
  readBuffer = new Uint8Array(0);
  bytesReceived = 0;
  framesRendered = 0;
  previewFramesRendered = 0;
  rawBytesLabel.textContent = "No data yet.";
  setConnectionUi(false);
  setStatus("Disconnected");
}

async function connectSerial() {
  if (!("serial" in navigator)) {
    setStatus("Web Serial API not supported in this browser");
    return;
  }

  try {
    port = await navigator.serial.requestPort();
    await port.open({ baudRate: BAUD_RATE, bufferSize: FRAME_SIZE + 2 });
    keepReading = true;
    readBuffer = new Uint8Array(0);
    bytesReceived = 0;
    framesRendered = 0;
    previewFramesRendered = 0;
    rawBytesLabel.textContent = "Waiting for data...";
    setConnectionUi(true);
    setStatus("Connected, waiting for frames");
    await readLoop();
    if (port) {
      await disconnectSerial();
    }
  } catch (error) {
    console.error(error);
    keepReading = false;
    setConnectionUi(false);
    setStatus(`Error: ${error.message}`);
  }
}

connectButton.addEventListener("click", () => {
  connectSerial();
});

disconnectButton.addEventListener("click", () => {
  disconnectSerial();
});

navigator.serial?.addEventListener("disconnect", async () => {
  await disconnectSerial();
  setStatus("Device disconnected");
});

navigator.serial?.addEventListener("connect", () => {
  if (!port) {
    setStatus("Device detected, ready to connect");
  }
});
