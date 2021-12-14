# SuperMario 32
A C implementation of Super Mario Bros for the NES using the STM32F091RCT6 chip. Uses the MSP2202 240x320 display for output and SD card storage.

# Specifications and Features
* STM32F09RCT6 chip running at 48Mhz with 32Kb RAM and 256Kb flash
* MSP2202 240x320 Display
* 40 FPS at 180x128 resolution, fullscreen achieves around 10-15 FPS
* Tile buffer frame rendering
* DMA SPI Communication with buffer swapping
* Midi sound output using DAC and timers (significant performance decrease)

# Video
https://user-images.githubusercontent.com/61773460/146089656-76f0ddeb-f2e8-439a-b112-3323f38f9a99.MOV



# Usage
This project uses System Workbench to flash and debug the STM32F091RCT6 and as such this project can be droppped into a System Workbench workspace to run on any F091RCT6 chip.
