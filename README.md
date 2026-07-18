# basic_tank

A small autonomous ESP32 tank rover that drives forward, detects obstacles with a common HC-SR04 ultrasonic sensor, backs up, turns, and tries again.

## What you need

| Qty | Part | Notes |
|---:|---|---|
| 1 | ESP32 DevKit V1 | Classic ESP32/WROOM-32 style board |
| 1 | HC-SR04 | Four-pin ultrasonic module from most starter kits |
| 1 | TB6612FNG driver | More efficient than an L298N; a DRV8833 can work after rewiring/code changes |
| 2 | N20 micro metal gearmotors | Use a matched pair; note their rated voltage, gear ratio/RPM, and stall current |
| 1 | N20-compatible tank chassis | Needs 12 mm N20 clamps and wheels/sprockets matching the motor shaft |
| 1 | Protected battery pack | Voltage must suit your motors and driver |
| 1 | Regulated 5 V supply | Buck converter rated for ESP32 current spikes |
| 1 each | 1 kΩ and 2 kΩ resistor | Echo voltage divider (similar 1:2 values are fine) |
| — | Switch, wire, standoffs, capacitors | 0.1 µF ceramic across each motor helps suppress noise |

> **Power warning:** Never run the motors from the ESP32's 3.3 V/5 V pin. Match the motor supply voltage to the motors, use a protected battery pack and suitable regulator, and connect every ground together. Do not connect raw lithium cells without an appropriate protection/charging arrangement.

N20 describes the gearbox size, not one electrical specification. N20 motors are sold in several voltages, gear ratios, and speeds. Check the listing or motor label before choosing the battery. The TB6612FNG suits many small N20 motors, but verify that the motor's **stall current per channel** is within the driver breakout's limits; tiny motors can still pull rude amounts of current when jammed.

## Quick start

1. Follow [the wiring guide](docs/WIRING.md), especially the Echo voltage divider.
2. Install [PlatformIO](https://platformio.org/install).
3. Connect the ESP32 by USB and run `pio run -t upload`.
4. Put the tank on a stand with its treads clear of the table. N20 drivetrains can be surprisingly quick.
5. Run `pio device monitor` at 115200 baud and verify range readings and tread direction.
6. If a tread is reversed, change its `INVERT_*_MOTOR` value in `include/config.h`.
7. Set it on the floor with space around it and switch on motor power.

The complete physical build is in [BUILD.md](docs/BUILD.md). Behavior and calibration are in [TUNING.md](docs/TUNING.md).

## Modular firmware

This stays Arduino/C++, but it is not one giant sketch. Motor channels, tank drive, ultrasonic sensing, navigation, and application scheduling are separate modules. `main.cpp` only selects parts and connects them, so later hardware changes stay local. See [the architecture and extension guide](docs/ARCHITECTURE.md) for servo scanning, Bluetooth/Wi-Fi control, encoders, and additional sensors.

## Default behavior

The rover takes five range samples and uses their median. It cruises while clear. At 25 cm it stops, reverses, pivots left or right (alternating each escape), then cautiously resumes. Four consecutive invalid sample groups cause a fail-safe stop; valid readings recover it.

All pins, speeds, distances, and timings are in [`include/config.h`](include/config.h).

## Pin map

| ESP32 | Connects to | Purpose |
|---|---|---|
| GPIO 16 / 17 / 18 | AIN1 / AIN2 / PWMA | Left motor direction/PWM |
| GPIO 19 / 21 / 22 | BIN1 / BIN2 / PWMB | Right motor direction/PWM |
| GPIO 23 | STBY | Driver enable |
| GPIO 25 | HC-SR04 Trig | Trigger output |
| GPIO 34 | Divider output from Echo | Range input (input-only pin) |
| 3.3 V | TB6612 VCC | Driver logic supply |
| 5 V regulated | HC-SR04 VCC + ESP32 5V/VIN | Sensor/controller supply |
| Motor supply + | TB6612 VM | Motor power only |
| GND | Every module/battery ground | Common reference |

## Development

```sh
pio test -e native
pio run -e esp32dev
```

The navigation and drive-model logic are hardware-independent and unit tested on the host. GPIO access stays inside the motor and sensor drivers.

## License

MIT — see [LICENSE](LICENSE).
