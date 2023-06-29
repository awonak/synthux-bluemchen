#include <string>

#include "../../kxmx_bluemchen/src/kxmx_bluemchen.h"
#include "daisysp.h"
#include "flt.h"
#include "vox.h"

using namespace kxmx;
using namespace daisy;
using namespace daisysp;

Bluemchen bluemchen;

Parameter knob1;
Parameter knob2;
Parameter cv1;
Parameter cv2;

const int kVoxCount = 15;
const float kVoxVolumeKof = 3.0 / kVoxCount;
Vox voxs[kVoxCount];
Filter flt;

uint32_t last_ui_update = 0;
uint32_t refresh_ms = 100;

void UpdateControls() {
    knob1.Process();
    knob2.Process();
    cv1.Process();
    cv2.Process();

    // TODO: store all CV values and create a peripheral routing map.

    for (auto& vox : voxs) {
        vox.Update(knob1.Value(), knob2.Value());
    }

    flt.SetTimbre(cv1.Value());
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

    str = std::to_string(static_cast<int>(knob1.Value() * 100));
    bluemchen.display.SetCursor(26, 0);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = " k2:";
    bluemchen.display.SetCursor(0, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = std::to_string(static_cast<int>(knob2.Value() * 100));
    bluemchen.display.SetCursor(26, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = "cv1: ";
    bluemchen.display.SetCursor(0, 16);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = std::to_string(static_cast<int>(cv1.Value() * 100));
    bluemchen.display.SetCursor(26, 16);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = "cv2:";
    bluemchen.display.SetCursor(0, 24);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = std::to_string(static_cast<int>(cv2.Value() * 100));
    bluemchen.display.SetCursor(26, 24);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    bluemchen.display.Update();
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    for (size_t i = 0; i < size; i++) {
        float outputL, outputR = 0;
        for (int j = 0; j < kVoxCount; j++) {
            (i % 2)
                ? outputL += voxs[j].Process()
                : outputR += voxs[j].Process();
        }
        outputL = flt.Process(outputL) * kVoxVolumeKof;
        outputR = flt.Process(outputR) * kVoxVolumeKof;
        out[0][i] = outputL;
        out[1][i] = outputR;
    }
}

int main(void) {
    bluemchen.Init();
    bluemchen.StartAdc();

    knob1.Init(bluemchen.controls[bluemchen.CTRL_1], 0.0f, 1.0f, Parameter::LINEAR);
    knob2.Init(bluemchen.controls[bluemchen.CTRL_2], 0.0f, 1.0f, Parameter::LINEAR);

    cv1.Init(bluemchen.controls[bluemchen.CTRL_3], 0.0f, 1.0f, Parameter::LINEAR);
    cv2.Init(bluemchen.controls[bluemchen.CTRL_4], 0.0f, 1.0f, Parameter::LINEAR);

    float samplerate = bluemchen.AudioSampleRate();

    // VOX SETUP
    float voxFreq = Vox::kOscLowestFreq;
    for (auto& vox : voxs) {
        vox.Init(samplerate, voxFreq);
        voxFreq *= 1.5;
        if (voxFreq > 1500) voxFreq *= 0.5;
    }

    // FILTER SETUP
    flt.Init(samplerate);

    // Start Audio
    bluemchen.StartAudio(AudioCallback);

    while (1) {
        UpdateControls();
        UpdateOled();
    }
}
