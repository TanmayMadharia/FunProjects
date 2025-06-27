#include <Adafruit_NeoPixel.h>

#define LED_PIN     6
#define LED_COUNT   16
#define BRIGHTNESS  80

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

enum Mode {
  MODE_BRAKE,
  MODE_LEFT,
  MODE_RIGHT,
  MODE_OFF
};

Mode currentMode = MODE_BRAKE;
unsigned long lastModeChange = 0;
unsigned long modeDuration = 5000; // 5 seconds

// For blinkers
unsigned long lastStepTime = 0;
int blinkerIndex = 0;
unsigned long blinkerSpeed = 100; // time between each LED in the running effect (ms)

// For brake
bool brakeStarted = false;
unsigned long brakeStartTime = 0;

void setup() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();
}

void loop() {
  unsigned long now = millis();

  // Auto mode change every 5 seconds
  if (now - lastModeChange >= modeDuration) {
    currentMode = static_cast<Mode>((currentMode + 1) % 4);
    lastModeChange = now;

    // Reset mode-specific states
    blinkerIndex = 0;
    lastStepTime = now;
    brakeStarted = false;
    strip.clear();
    strip.show();
  }

  switch (currentMode) {
    case MODE_BRAKE:
      brakeMode();
      break;
    case MODE_LEFT:
      runningLeftBlinker();
      break;
    case MODE_RIGHT:
      runningRightBlinker();
      break;
    case MODE_OFF:
      strip.clear();
      strip.show();
      break;
  }

  delay(10); // small delay
}

void brakeMode() {
  if (!brakeStarted) {
    brakeStarted = true;
    brakeStartTime = millis();
  }

  unsigned long elapsed = millis() - brakeStartTime;

  if (elapsed < 1000) {
    bool flash = (elapsed / 100) % 2 == 0; //changing to smaller number flashes quickly
    setAll(flash ? strip.Color(255, 0, 0) : 0);
  } else {
    setAll(strip.Color(255, 0, 0));
  }
}

void runningLeftBlinker() {
  unsigned long now = millis();

  if (now - lastStepTime >= blinkerSpeed) {
    strip.clear();
    for (int i = 7; i >= 7 - blinkerIndex && i >= 0; i--) {
      strip.setPixelColor(i, strip.Color(255, 120, 0)); // Amber
    }
    strip.show();

    blinkerIndex++;
    if (blinkerIndex >= 8) {
      blinkerIndex = 0;
    }

    lastStepTime = now;
  }
}

void runningRightBlinker() {
  unsigned long now = millis();

  if (now - lastStepTime >= blinkerSpeed) {
    strip.clear();
    for (int i = 8; i <= 8 + blinkerIndex && i < 16; i++) {
      strip.setPixelColor(i, strip.Color(255, 120, 0)); // Amber
    }
    strip.show();

    blinkerIndex++;
    if (blinkerIndex >= 8) {
      blinkerIndex = 0;
    }

    lastStepTime = now;
  }
}

void setAll(uint32_t color) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}
