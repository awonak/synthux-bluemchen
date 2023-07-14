# SynthUX Academy workshops on kxmx_bluemchen hardware

## Quick Start

This assumes you have already completed [setting up your development environment](https://github.com/electro-smith/DaisyWiki/wiki/1.-Setting-Up-Your-Development-Environment) for Daisy.

Clone the repository with the recursive flag to also fetch the submodules:

```shell
git clone --recursive https://github.com/awonak/synthux-bluemchen
```

Open the project in `vscode`:

```shell
cd synthux-bluemchen
code .
```

Navigate to the file `apps/blink/blink.cpp`.

Use the default build task (`build_and_flash`) to compile the current file and flash it to the Daisy in DFU mode.

```plaintext
Ctrl + Shift + B
```

## Workshops

### [Drone](apps/drone/)

* [Workshop](https://www.synthux.academy/workshop/build-a-multi-voice-drone-synth)
* [Source Code](https://github.com/Synthux-Academy/simple-examples/tree/main/daisyduino/simple-multivoice-drone)
* [YouTube Video](https://www.youtube.com/live/OupvaCWw7is?feature=share)

### [Sampler/Looper](apps/looper/)

* [Workshop](https://www.synthux.academy/workshop/build-your-own-sampler-looper)
* [Source Code](https://github.com/Synthux-Academy/simple-examples/tree/main/daisyduino/simple-looper)
* [YouTube Video](https://www.youtube.com/live/X3hh8FNe_GE)
