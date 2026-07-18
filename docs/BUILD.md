# Build guide

## 1. Mechanical assembly

1. Fit both gear motors firmly and parallel in the chassis.
2. Install drive wheels, idlers, and treads. Treads should move freely without sagging.
3. Mount the battery low and centered.
4. Mount the HC-SR04 at the front, level, with both transducers unobstructed. Soft material directly around it can absorb sound.
5. Mount the ESP32 and driver on standoffs. Prevent the underside from touching metal.

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

## 4. Floor test

Start on a large, flat floor away from stairs, pets, and feet. Keep the power switch within reach. Tune at low speed before increasing it.

