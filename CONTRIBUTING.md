# Contributing

Open an issue before a large change. Keep hardware behavior configurable, preserve the sensor fail-safe, and add native tests for navigation changes.

Before submitting:

```sh
pio test -e native
pio run -e esp32dev
```

Document any changed wiring prominently. Never recommend direct 5 V signals into ESP32 GPIO or powering motors from the ESP32 board.

