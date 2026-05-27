#pragma once

#include <Arduino.h>

namespace wifi_setup {
    // Blocking: brings up the captive-portal AP, blocks until the user submits the
    // form, persists the new symbols/dwell to storage, then reboots. Does not return.
    [[noreturn]] void run_portal();

    // Wipes wifi credentials AND symbol/dwell prefs.
    void factory_reset();

    // Tries to connect to the saved network within `timeout_ms`. Returns true on success.
    bool connect(uint32_t timeout_ms);
}
