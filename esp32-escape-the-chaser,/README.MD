# 🎮 Escape the Chaser

A beginner-friendly hobby project: an ESP32, an OLED and a joystick make a tiny game.
You control a dot with the joystick, another dot chases you, and you have to reach the
**bottom-right corner** without getting caught. Caught = game over. Reach the goal = you win.

## 🧰 Parts list

- 1x ESP32 dev board
- 1x 0.96" SSD1306 OLED, 128x64, I2C (4 pins)
- 1x analog joystick module (GND, +5V, VRx, VRy, SW)
- 1x breadboard + jumper wires
- 1x USB cable

## 🔌 Wiring

**OLED (I2C)**

| OLED pin | ESP32 pin |
|----------|-----------|
| VCC      | 3V3       |
| GND      | GND       |
| SDA      | GPIO 21   |
| SCL      | GPIO 22   |

**Joystick**

| Joystick pin | ESP32 pin |
|--------------|-----------|
| GND          | GND       |
| +5V (VCC)    | **3V3**   |
| VRx          | GPIO 34   |
| VRy          | GPIO 35   |
| SW (button)  | GPIO 32   |

> ⚠️ Power the joystick from **3V3**, not 5V, even though the pin says +5V.
> The ESP32 ADC pins can't take more than 3.3V.

## ⚙️ Setup

1. Install [Arduino IDE](https://www.arduino.cc/en/software) (2.x recommended).
2. Add ESP32 board support:
   - File → Preferences → Additional Boards Manager URLs, add:
     `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
   - Tools → Board → Boards Manager → search **esp32** → install the one by Espressif.
3. Install libraries (Sketch → Include Library → Manage Libraries):
   - **Adafruit SSD1306**
   - **Adafruit GFX Library**
   - Click **Install all** if it asks about dependencies (Adafruit BusIO).
4. Open `escape_the_chaser/escape_the_chaser.ino`.
5. Tools → Board → **ESP32 Dev Module**, and select your COM port.
6. Click **Upload**. If it hangs on "Connecting...", hold the **BOOT** button on the ESP32.

## 🕹️ How to play

- Move the dot with the joystick. **Don't touch the stick while the board powers on**
  because it calibrates the centre position at boot.
- The chaser starts moving after a 1 second head start.
- Reach the box in the **bottom-right corner** to win 🏆
- If the chaser touches you, it's **GAME OVER** 💀
- Press the joystick button to restart.

## 💡 Tips

**Tuning the game** (constants at the top of the sketch)

| Constant       | What it does                                           |
|----------------|--------------------------------------------------------|
| `CHASER_SPEED` | Difficulty. ~0.6 easy, 0.9 normal, 1.2+ brutal         |
| `PLAYER_SPEED` | How fast you move                                      |
| `START_DELAY`  | Your head start in ms                                  |
| `CATCH_DIST`   | How close the chaser needs to get to catch you         |

**Troubleshooting**

- **OLED is blank:** check SDA/SCL first. Then try address `0x3D` in `OLED_ADDR`,
  or run an "I2C Scanner" sketch to find yours.
- **"OLED not found" in Serial Monitor:** usually a loose jumper wire or SDA/SCL swapped.
- **Dot drifts by itself:** raise `DEADZONE` (try 400-500) and keep hands off the stick at boot.
- **Controls reversed:** flip the sign on that axis in the `px +=` / `py +=` line,
  or swap the joystick wires.
- **Upload fails:** wrong port, charge-only USB cable, or you need to hold BOOT.

**Good habits**

- Double-check power and GND before plugging in.
- Change one thing at a time, then test.
- Use `Serial.println()` to debug, e.g. print raw joystick values.

## 🚀 Ideas to level it up

- Make the chaser speed up over time
- Add walls or obstacles
- Add a timer / score (faster win = better score)
- Add a buzzer for win and game over sounds
- Add a second chaser or a hard mode
- Show "LEVEL 2" between rounds

## 🤝 Contributing

Built something cool on top of this? Open a PR or share it in the group!

## 📄 License

MIT, see [LICENSE](LICENSE).