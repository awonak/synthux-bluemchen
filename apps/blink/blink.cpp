#include "../../kxmx_bluemchen/src/kxmx_bluemchen.h"

using namespace daisy;

DaisySeed hardware;

int main(void) {
// Declare a variable to store the state we want to set for the LED.
    bool led_state;
    led_state = true;

    // Configure and Initialize the Daisy Seed
    // These are separate to allow reconfiguration of any of the internal
    // components before initialization.
    hardware.Configure();
    hardware.Init();

    // Loop forever
    for(;;)
    {
        // Set the onboard LED
        hardware.SetLed(led_state);

        // Toggle the LED state for the next time around.
        led_state = !led_state;

        // Wait 50ms
        System::Delay(500);
    }
}
