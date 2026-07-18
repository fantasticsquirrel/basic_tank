#pragma once

#if __has_include("secrets.h")
#include "secrets.h"
#else
// Allows CI builds without credentials. Wi-Fi remains disabled until the user
// copies secrets.example.h to secrets.h and supplies real local values.
namespace secrets {
constexpr char WIFI_SSID[] = "";
constexpr char WIFI_PASSWORD[] = "";
constexpr char CONTROL_TOKEN[] = "";
}  // namespace secrets
#endif

