void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
// ESP32 + SSD1306 OLED + Joystick "Escape the Chaser"
// Move the dot with the joystick, dodge the chaser, reach the bottom-right corner to win.
// Libraries: Adafruit SSD1306, Adafruit GFX (install via Library Manager)

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_W 128
#define SCREEN_H 64
#define OLED_ADDR 0x3C

// Wiring
// OLED: VCC->3V3, GND->GND, SDA->GPIO21, SCL->GPIO22
// Joystick: VCC->3V3, GND->GND, VRx->GPIO34, VRy->GPIO35, SW->GPIO32
#define JOY_X 34
#define JOY_Y 35
#define JOY_SW 32

Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);

// ---- tweak these to change difficulty ----
const float PLAYER_SPEED = 2.2;   // max player speed (px/frame)
const float CHASER_SPEED = 0.9;   // chaser speed (higher = harder)
const int   CATCH_DIST   = 4;     // how close the chaser must get to catch you
const int   START_DELAY  = 1000;  // ms before chaser starts moving
// ------------------------------------------

const int DEADZONE = 300;
const int BORDER = 3;

// goal box in the bottom-right corner
const int GOAL_SIZE = 10;
const int GOAL_X = SCREEN_W - BORDER - GOAL_SIZE;
const int GOAL_Y = SCREEN_H - BORDER - GOAL_SIZE;

enum State { PLAYING, WON, LOST };
State state = PLAYING;

float px, py;      // player
float cx, cy;      // chaser
int centerX, centerY;
unsigned long startTime;

void resetGame() {
  px = 8;  py = 8;                          // player starts top-left
  cx = SCREEN_W - 10; cy = 10;              // chaser starts top-right
  state = PLAYING;
  startTime = millis();
}

float readAxis(int pin, int center) {
  int d = analogRead(pin) - center;
  if (abs(d) < DEADZONE) return 0;
  float range = (d > 0) ? (4095 - center) : center;
  return constrain(d / range, -1.0, 1.0);
}

void showMessage(const char* line1, const char* line2) {
  display.clearDisplay();
  display.drawRect(0, 0, SCREEN_W, SCREEN_H, SSD1306_WHITE);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(line1, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_W - w) / 2, 14);
  display.print(line1);
  display.setTextSize(1);
  display.getTextBounds(line2, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_W - w) / 2, 44);
  display.print(line2);
  display.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(JOY_SW, INPUT_PULLUP);

  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found, check wiring");
    while (true) delay(10);
  }

  // calibrate joystick centre (don't touch the stick at boot)
  long sx = 0, sy = 0;
  for (int i = 0; i < 32; i++) {
    sx += analogRead(JOY_X);
    sy += analogRead(JOY_Y);
    delay(5);
  }
  centerX = sx / 32;
  centerY = sy / 32;

  resetGame();
}

void loop() {
  if (state != PLAYING) {
    showMessage(state == WON ? "YOU WIN!" : "GAME OVER", "press stick to retry");
    if (digitalRead(JOY_SW) == LOW) {
      delay(250);
      resetGame();
    }
    delay(20);
    return;
  }

  // move player
  px += readAxis(JOY_X, centerX) * PLAYER_SPEED;
  py += readAxis(JOY_Y, centerY) * PLAYER_SPEED;
  px = constrain(px, BORDER + 2, SCREEN_W - BORDER - 3);
  py = constrain(py, BORDER + 2, SCREEN_H - BORDER - 3);

  // move chaser toward player (after a short head start for the player)
  if (millis() - startTime > START_DELAY) {
    float dx = px - cx, dy = py - cy;
    float dist = sqrt(dx * dx + dy * dy);
    if (dist > 0.01) {
      cx += (dx / dist) * CHASER_SPEED;
      cy += (dy / dist) * CHASER_SPEED;
    }
  }

  // lose check
  float ddx = px - cx, ddy = py - cy;
  if (ddx * ddx + ddy * ddy < CATCH_DIST * CATCH_DIST) {
    state = LOST;
    return;
  }

  // win check (player inside goal box)
  if (px >= GOAL_X && py >= GOAL_Y) {
    state = WON;
    return;
  }

  // draw
  display.clearDisplay();
  display.drawRect(0, 0, SCREEN_W, SCREEN_H, SSD1306_WHITE);
  display.drawRect(1, 1, SCREEN_W - 2, SCREEN_H - 2, SSD1306_WHITE);
  display.drawRect(GOAL_X, GOAL_Y, GOAL_SIZE, GOAL_SIZE, SSD1306_WHITE); // goal
  display.fillCircle((int)px, (int)py, 2, SSD1306_WHITE);                // player
  display.fillCircle((int)cx, (int)cy, 2, SSD1306_WHITE);                // chaser
  display.display();

  delay(15);
}