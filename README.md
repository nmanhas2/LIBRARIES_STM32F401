Bare-metal STM32 Driver Libraries With Tests

Using this NUCLEO Dev-board: https://www.st.com/en/evaluation-tools/nucleo-f401re.html

This repo contains libraries within the Inc folders of each project that can be used for GPIO, UART, ADC, etc, functionalities for the STM32F401RE MCU. There are tests included within each project file, mostly using the on-board LED and button for this particular nucleo board. Most of the tests should only be run one at a time. 

Used this to obtain the header ARM package CMSIS header files needed: https://www.st.com/en/embedded-software/stm32cubef4.html. You only need Drivers -> CMSIS -> Device and Include folders (for whichever MCU you're using). Since this is intended STM32F4x MCU, I only included the necessary header files for that MCU in the chip_headers/CMSIS folder. 

If there are issues with detecting the header files, you may need to right click on the project and go to Properties: 

![image](https://github.com/nmanhas2/LIBRARIES_STM32F401/assets/113201793/f02c80f8-7cd4-4942-8917-d898fd356037)

Then go to C/C++ General -> Paths and Symbols:

![image](https://github.com/nmanhas2/LIBRARIES_STM32F401/assets/113201793/71d7f93d-65f1-4ae1-8867-54b0bcc337cf)

Then click Add -> File system, and head to the chip_headers/CMSIS folder and select the Include folder, then do the same for the Device folder:

![image](https://github.com/nmanhas2/LIBRARIES_STM32F401/assets/113201793/26bcb2b1-232e-4fda-8dc6-178ffac93335)

I've also included resources like the MCU datasheet, reference manual, etc., that I used. 
