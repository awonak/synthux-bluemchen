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
