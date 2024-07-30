#include <string>

#include "daisy_patch.h"
#include "daisysp.h"
#include "kxmx_bluemchen.h"

using namespace daisy;
using namespace daisysp;
using namespace kxmx;

// Hardware
Bluemchen bluemchen;

Parameter knob1;
Parameter knob2;

// Daisy objects
ReverbSc DSY_SDRAM_BSS verb;

// State variables
static float mixLevel, feedback;

// UI helpers
const int refresh_ms = 50;
uint32_t last_ui_update = 0;

void UpdateControls() {
    mixLevel = knob1.Process();
    feedback = knob2.Process();

    verb.SetFeedback(0.8f + feedback * .199f);
}

void UpdateOled() {
    // limit update rate
    uint32_t now = bluemchen.seed.system.GetNow();
    if (now - last_ui_update < refresh_ms) {
        return;
    }
    last_ui_update = now;

    bluemchen.display.Fill(false);
    std::string str = "";
    char* cstr = &str[0];

    str = " Mix: ";
    bluemchen.display.SetCursor(0, 0);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    bluemchen.display.DrawRect(4, 10, 62, 15, 1, 0);
    bluemchen.display.DrawRect(4, 10, 4+ mixLevel * 58, 15, 1, 1);

    str = " Feedback: ";
    bluemchen.display.SetCursor(0, 18);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    bluemchen.display.DrawRect(4, 26, 62, 31, 1, 0);
    bluemchen.display.DrawRect(4, 26, 4 + feedback * 58, 30, 1, 1);

    bluemchen.display.Update();
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
  static float dryL, dryR, verbL, verbR;

  for (size_t i = 0; i < size; i++) {
    dryL = in[0][i];
    dryR = in[1][i];
   
    verb.Process(dryL, dryR, &verbL, &verbR);

    out[0][i] = (dryL * (1 - mixLevel)) + verbL * mixLevel;
    out[1][i] = (dryR * (1 - mixLevel)) + verbR * mixLevel;
  }
}

int main(void) {
    bluemchen.Init();
    bluemchen.StartAdc();

    knob1.Init(bluemchen.controls[bluemchen.CTRL_1], 0.0f, 1.0f, Parameter::LINEAR);
    knob2.Init(bluemchen.controls[bluemchen.CTRL_2], 0.0f, 1.0f, Parameter::LINEAR);

    float sample_rate = bluemchen.AudioSampleRate();

    // Initialize reverb
    verb.Init(sample_rate);
    verb.SetFeedback(0.95f);
    verb.SetLpFreq(18000.0f);

    mixLevel = 0.1f;

    // Start Audio
    bluemchen.StartAudio(AudioCallback);

    while (1) {
        UpdateControls();
        UpdateOled();
        bluemchen.DelayMs(refresh_ms);
    }
}
