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
320x128 Resolution

https://user-images.githubusercontent.com/61773460/146091651-dc093606-52f0-42df-a64e-7474da025505.mp4

160x128 Resolution


https://user-images.githubusercontent.com/61773460/146092201-2b70cdb3-aced-4f01-9e4c-67000f5abc5e.mov




# Usage
This project uses System Workbench to flash and debug the STM32F091RCT6 and as such this project can be droppped into a System Workbench workspace to run on any F091RCT6 chip.
