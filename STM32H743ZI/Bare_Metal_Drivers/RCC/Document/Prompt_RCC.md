- I want to develop the RCC/Clockdriver for the STM32H743ZIT6 controller.
- I need you to understand the RCC register structure, RCC modes and other important imformation about RCC from the below documents placed in the repository.
    1> \Document\Reference_Manual_stm32h742-stm32h743-753_Reset_And_Clock_Controll(RCC)pdf.pdf
    2> \Document\Reference_Manual_stm32h742-stm32h743-Mem_Addresses.pdf

- Please provide the RCC/Clock driver code for the STM32H743ZIT6 controller.
- Please note that the driver code should not have anny other library dependencies like (CMSIS, HAL, etc)
- I want you to generate the RCCH743ZI.c anf RCCH743ZI.h files for the driver code inside the folder "\RCC".
- and the example codes inside the folder "\RCC\Examples"
- Please make the code as simple as possible, modular and easy to understand.
- Generate the APIs for different peripheral clock enable and configuration also (like SPI, I2C, GPIO etc..). You can refer to the given document for the supported list of peripherals.

- Also provide the example codes for the possible clock configurations like below.
1> Running on system clock speed of 4MHz.
    a> HSI clock source
    b> HSE clock source
    c> LSI clock source
2> Running on system clock speed of 16MHz.
    a> HSI clock source
    b> HSE clock source
    c> LSI clock source
3> Running on system clock speed of 480MHz (Maximum system clock speed)
    a> HSI clock source
    b> HSE clock source
    c> MSI clock source if possible.