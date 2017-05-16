; LCD.s
; Student names: Kenneth Lin, Kunpeng Qin
; Last modification date: Mar. 23, 2017

; Runs on LM4F120/TM4C123
; Use SSI0 to send an 8-bit code to the ST7735 160x128 pixel LCD.

; As part of Lab 7, students need to implement these LCD_WriteCommand and LCD_WriteData
; This driver assumes two low-level LCD functions

; Backlight (pin 10) connected to +3.3 V
; MISO (pin 9) unconnected
; SCK (pin 8) connected to PA2 (SSI0Clk)
; MOSI (pin 7) connected to PA5 (SSI0Tx)
; TFT_CS (pin 6) connected to PA3 (SSI0Fss)
; CARD_CS (pin 5) unconnected
; Data/Command (pin 4) connected to PA6 (GPIO)
; RESET (pin 3) connected to PA7 (GPIO)
; VCC (pin 2) connected to +3.3 V
; Gnd (pin 1) connected to ground

;***************************************************
; This is a library for the Adafruit 1.8" SPI display.
; This library works with the Adafruit 1.8" TFT Breakout w/SD card
; ----> http://www.adafruit.com/products/358
; as well as Adafruit raw 1.8" TFT display
; ----> http://www.adafruit.com/products/618
;
; Check out the links above for our tutorials and wiring diagrams
; These displays use SPI to communicate, 4 or 5 pins are required to
; interface (RST is optional)
; Adafruit invests time and resources providing this open source code,
; please support Adafruit and open-source hardware by purchasing
; products from Adafruit!
;
; Written by Limor Fried/Ladyada for Adafruit Industries.
; MIT license, all text above must be included in any redistribution
;****************************************************

DC                      EQU   0x40004100
DC_COMMAND              EQU   0
DC_DATA                 EQU   0x40
SSI0_DR_R               EQU   0x40008008
SSI0_SR_R               EQU   0x4000800C
SSI_SR_RNE              EQU   0x00000004  ; SSI Receive FIFO Not Empty
SSI_SR_BSY              EQU   0x00000010  ; SSI Busy Bit
SSI_SR_TNF              EQU   0x00000002  ; SSI Transmit FIFO Not Full

    EXPORT   writecommand
    EXPORT   writedata

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB
    ALIGN

; *****************************************************************
; The Data/Command pin must be valid when the eighth bit is
; sent.  The SSI module has hardware input and output FIFOs
; that are 8 locations deep.  Based on the observation that
; the LCD interface tends to send a few commands and then a
; lot of data, the FIFOs are not used when writing
; commands, and they are used when writing data.  This
; ensures that the Data/Command pin status matches the byte
; that is actually being transmitted.
; The write command operation waits until all data has been
; sent, configures the Data/Command pin for commands, sends
; the command, and then waits for the transmission to
; finish.
; The write data operation waits until there is room in the
; transmit FIFO, configures the Data/Command pin for data,
; and then adds the data to the transmit FIFO.
; NOTE: These functions will crash or stall indefinitely if
; the SSI0 module is not initialized and enabled.
; *****************************************************************

; *****************************************************************
; ************************* writecommand **************************
; This is a helper function that sends an 8-bit command to the LCD.
; Input: R0  8-bit command to transmit
; Output: none
; Assumes: SSI0 and port A have already been initialized and enabled
writecommand
    ; 1) Read SSI0_SR_R and check bit 4.
st1 LDR R1, =SSI0_SR_R                  ; Read SSIO_SR_R.
    LDR R2, [R1]                        ; Read SSIO_SR_R.
    AND R2, #SSI_SR_BSY                 ; Mask out the 4th bit. 
    CMP R2, #SSI_SR_BSY                 ; Compare the masked result with high. 
    ; 2) If bit 4 is high, loop back to step 1 (wait for BUSY bit to be low).
    BEQ st1                             ; If it is high, go back to step 1. 
    ; 3) Clear D/C=PA6 to zero.         ; Clear DC. 
    MOV R2, #DC_COMMAND                 ; Clear DC. 
    LDR R3, =DC                         ; Clear DC. 
    STR R2, [R3]                        ; We can directly move 0 in because it is bit-addressing and is thus friendly. 
    ; 4) Write the command to SSI0_DR_R.; Write Command to SSI0_DR_R. 
    LDR R3, =SSI0_DR_R                  ; Write Command to SSI0_DR_R. 
    STR R0, [R3]                        ; Write Command to SSI0_DR_R. 
    ; 5) Read SSI0_SR_R and check bit 4.; Read SSIO_SR_R. 
st5 LDR R1, =SSI0_SR_R                  ; Read SSIO_SR_R.
    LDR R2, [R1]                        ; Read SSIO_SR_R.
    AND R2, #SSI_SR_BSY                 ; Mask out the 4th bit. 
    CMP R2, #SSI_SR_BSY                 ; Compare the masked result with high.  
    ; 6) If bit 4 is high, loop back to step 5 (wait for BUSY bit to be low).
    BEQ st5                             ; If it is high, go back to step 5. 
    BX  LR                              ; Return. 

; *****************************************************************
; *************************** writedata ***************************
; This is a helper function that sends an 8-bit data to the LCD.
; Input: R0  8-bit data to transmit BUT use STR. 
; Output: none
; Assumes: SSI0 and port A have already been initialized and enabled
writedata
    ; 1) Read SSI0_SR_R and check bit 1, 
sp1 LDR R1, =SSI0_SR_R                  ; Read SSIO_SR_R.
    LDR R2, [R1]                        ; Read SSIO_SR_R.
    AND R2, #SSI_SR_TNF                 ; Mask out the 4th bit. 
    CMP R2, #SSI_SR_TNF                 ; Compare the masked result with high. 
    ; 2) If bit 1 is low loop back to step 1 (wait for TNF bit to be high)
    BNE sp1                             ; If it is low, go back to step 1. 
    ; 3) Set D/C=PA6 to one             ; Set DC to data mode. 
    MOV R2, #DC_DATA                    ; Clear DC. 
    LDR R3, =DC                         ; Clear DC. 
    STR R2, [R3]                        ; We can directly move 0 in because it is bit-addressing and is thus friendly. 
    ; 4) Write the 8-bit data to SSI0_DR_R using STR
    LDR R3, =SSI0_DR_R                  ; Write data to SSI0_DR_R. 
    STR R0, [R3]                        ; Write data to SSI0_DR_R. 
    BX  LR                              ; Return. 

    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file