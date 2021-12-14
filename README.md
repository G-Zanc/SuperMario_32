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



https://user-images.githubusercontent.com/61773460/146092529-6428ab64-96f6-4597-b58a-779865011896.mp4



160x128 Resolution



https://user-images.githubusercontent.com/61773460/146092525-8fecac89-07fa-46b8-bad7-54b007eadc69.mov


320x240 Resolution (Full Screen)



https://user-images.githubusercontent.com/61773460/146092847-fbca078f-3f83-41d4-88f3-8c7db691b7f3.mp4



# Usage
This project uses System Workbench to flash and debug the STM32F091RCT6 and as such this project can be droppped into a System Workbench workspace to run on any F091RCT6 chip.
