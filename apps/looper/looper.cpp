#include "looper.h"

#include <algorithm>

#include "daisysp.h"
#include "kxmx_bluemchen.h"

using namespace kxmx;
using namespace daisy;
using namespace daisysp;

Bluemchen bluemchen;

Parameter loop_start;   // Knob 1
Parameter loop_length;  // Knob 2

// Allocate buffer in SDRAM
static const uint32_t kBufferLengthSec = 5;
static const uint32_t kSampleRate = 48000;
static const size_t kBufferLenghtSamples = kBufferLengthSec * kSampleRate;
static float DSY_SDRAM_BSS buffer[kBufferLenghtSamples];

static synthux::Looper looper;
static PitchShifter pitch_shifter;

// Keep track of pitch value selected from the encoder.
int8_t pitch_val;
// State variables for tracking OLED refresh rate.
const int refresh_ms = 100;
uint32_t last_ui_update = 0;


void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    for (size_t i = 0; i < size; i++) {
        float looper_out = looper.Process(in[1][i]);
        out[0][i] = out[1][i] = pitch_shifter.Process(looper_out);
    }
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

    str = " k1: ";
    bluemchen.display.SetCursor(0, 0);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = std::to_string(static_cast<int>(loop_start.Value() * 100));
    bluemchen.display.SetCursor(26, 0);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = " k2:";
    bluemchen.display.SetCursor(0, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = std::to_string(static_cast<int>(loop_length.Value() * 100));
    bluemchen.display.SetCursor(26, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = " enc:";
    bluemchen.display.SetCursor(0, 16);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = std::to_string(pitch_val);
    bluemchen.display.SetCursor(26, 16);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = " len:";
    bluemchen.display.SetCursor(0, 26);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = std::to_string(static_cast<int>(looper.LoopLength()));
    bluemchen.display.SetCursor(26, 26);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    bluemchen.display.Update();
}

int main(void) {
    bluemchen.Init();
    bluemchen.StartAdc();

    loop_start.Init(bluemchen.controls[bluemchen.CTRL_1], 0.0f, 1.0f, Parameter::LINEAR);
    loop_length.Init(bluemchen.controls[bluemchen.CTRL_2], 0.0f, 1.0f, Parameter::LINEAR);

    bluemchen.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    float samplerate = bluemchen.AudioSampleRate();

    // Setup looper
    looper.Init(buffer, kBufferLenghtSamples);

    // Setup pitch shifter
    pitch_shifter.Init(samplerate);

    bluemchen.StartAudio(AudioCallback);

    for (;;) {
        bluemchen.ProcessAllControls();

        // Set loop parameters
        looper.SetLoop(loop_start.Value(), loop_length.Value());

        // Toggle record
        looper.SetRecording(bluemchen.encoder.RisingEdge());

        // Set pitch - clamp value within a ±12 semitone range.
        int8_t read = bluemchen.encoder.Increment();
        pitch_val = std::clamp(pitch_val + read, -12, 12);
        pitch_shifter.SetTransposition((float)pitch_val / (float)12);

        UpdateOled();
    }
}
