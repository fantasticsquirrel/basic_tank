# Build guide

## 1. Mechanical assembly

1. Fit both N20 gearmotors in proper 12 mm clamps. Do not rely on zip ties around the gearbox or clamp the motor's thin rear can hard enough to deform it.
2. Install N20-shaft-compatible drive wheels, idlers, and treads. Many N20s use a 3 mm D-shaped output shaft, but measure yours before ordering hubs. Treads should move freely without sagging.
3. Mount the battery low and centered.
4. Mount the HC-SR04 at the front, level, with both transducers unobstructed. Soft material directly around it can absorb sound.
5. Mount the ESP32 and driver on standoffs. Prevent the underside from touching metal.

Before wiring, record the voltage, gearbox ratio or output RPM, and stall current from the motor listing. `N20` alone does not identify those values. Use a matched left/right pair.

## 2. Electrical assembly

1. Leave battery and USB disconnected.
2. Wire the TB6612 logic pins and motors using [WIRING.md](WIRING.md).
3. Build and meter-check the Echo divider before connecting GPIO 34.
4. Wire the regulated 5 V rail and motor `VM` rail.
5. Join all grounds. Inspect for reversed polarity and loose strands.
6. Add a master battery switch if possible.

## 3. Bench test

1. Raise the tank so the treads cannot move it.
2. Connect USB only. Upload firmware and verify sensible range data in the serial monitor.
3. Disconnect USB, connect motor/battery power, and keep the tank raised.
4. Confirm both treads drive forward. Toggle the corresponding inversion constant in `include/config.h` if not.
5. Place a flat object in front. Confirm the sequence is stop, reverse, pivot, forward.

Start with the conservative N20 PWM values provided. If the motors buzz but do not start, first check for binding and battery sag; then raise the speed gradually with the rover still lifted.

## 4. Floor test

Start on a large, flat floor away from stairs, pets, and feet. Keep the power switch within reach. Tune at low speed before increasing it.
