# Tuning and troubleshooting

Edit `include/config.h`, rebuild, and upload.

## Recommended order

1. Confirm motor inversion with the rover lifted.
2. Raise `CRUISE_SPEED` slowly from the conservative N20 default only if needed. Different N20 gear ratios can turn the same PWM value into either a crawl or a tiny tracked missile.
3. Adjust `STOP_DISTANCE_CM` so it stops before contact; 25–35 cm is typical.
4. Increase `REVERSE_TIME_MS` if it cannot clear corners.
5. Adjust `TURN_TIME_MS` until a pivot is roughly 70–110 degrees.
6. Keep `CLEAR_DISTANCE_CM` greater than `STOP_DISTANCE_CM` to prevent indecision.

## Symptoms

- **Always says `invalid`:** check 5 V sensor power, shared ground, Echo divider orientation, and GPIO numbers.
- **Reads too short/long:** aim at a large hard flat surface; fabric, angled surfaces, narrow legs, and very close objects are unreliable for ultrasound.
- **One tread goes backward:** flip `INVERT_LEFT_MOTOR` or `INVERT_RIGHT_MOTOR`.
- **Turns the wrong direction:** first correct motor inversion; do not swap navigation labels.
- **ESP32 resets when moving:** regulator or wiring voltage drop/noise. Add motor suppression and bulk capacitance, shorten power wiring, and use a regulator with adequate peak current.
- **Driver is hot:** stop immediately. Check stalled motor current, mechanical binding, motor voltage, and shorts. TB6612 is not suitable for motors whose stall current exceeds its rating.
- **Buzzes without moving:** battery sag, tread binding, insufficient motor current, or PWM below the motor's usable starting torque.
- **Runs far too fast:** the N20 gearbox is probably a high-RPM ratio. Lower all three speed constants; if useful low-speed control is impossible, use a lower-RPM geared pair rather than fighting it entirely in software.

Serial output marks a phase transition with `*`. A sensor fault deliberately stops the rover until valid measurements return.
