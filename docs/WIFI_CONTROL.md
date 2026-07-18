# Direct Wi-Fi control

The Android app talks directly to the ESP32 over the local Wi-Fi network. There is no cloud relay and no Internet dependency after installation.

## Configure the ESP32

1. Copy `include/secrets.example.h` to `include/secrets.h`.
2. Set your 2.4 GHz Wi-Fi SSID/password. Classic ESP32 does not use 5 GHz Wi-Fi.
3. Set a unique random control token of at least 12 characters. Use the same token in the app.
4. Upload firmware and open the 115200-baud serial monitor.
5. Note the printed `Tank control: http://...` IP address. A DHCP reservation in your router keeps it stable.

`include/secrets.h` is ignored by Git and must never be committed.

## Use the app

1. Put the Android phone on the same Wi-Fi network as the tank.
2. Enter the ESP32 IP and control token, then tap **Connect Direct**.
3. **Auto** runs ultrasonic obstacle avoidance.
4. **Manual** exposes independent left/right track controls. Drag upward for forward or downward for reverse; releasing a control returns that tread to zero.
5. **Stop** zeros both controls immediately.

The app sends manual commands every 140 ms. If the ESP32 receives no valid command for 450 ms, it commands both treads to zero. Switching modes also clears the previous manual command.

## Protocol

All endpoints require the shared token in `X-Control-Token`.

- `GET /api/status`
- `POST /api/mode?value=auto|manual`
- `POST /api/control?left=-255..255&right=-255..255&seq=<increasing integer>`

Motor values are clamped on the ESP32. Out-of-order/replayed sequence numbers are rejected. Status includes mode, distance, sensor validity, motion, phase, command freshness, last sequence, and IP.

## Security boundary

Traffic is plain HTTP because the app connects to a small local microcontroller. The token provides control authorization but is not encrypted in transit. Use this only on a trusted private Wi-Fi network, use a unique token, and do not expose ESP32 port 80 through the router. For untrusted networks, add a VPN or trusted gateway instead of forwarding the device directly.

## Build the Android APK

From `mobile/`:

```sh
npm install
npm run typecheck
npm run export:web
npm run build:apk
```

The EAS `preview` profile produces an installable APK. Android cleartext local-network access is enabled through Expo's build-properties plugin because the ESP32 serves HTTP rather than HTTPS.

