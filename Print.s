; Print.s
; Student names: Kenneth Lin, Kunpeng Qin
; Last modification date: Mar. 2017
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix
    EXPORT   Mod10

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
; Algorithm: 
; static void OutDec(int number) {
;   Stack s = new Stack(); 
;   int p = 0; 
;   int places = 0; 
;   while (number > 0) {
;     p = number % 10; 
;     s.push(p); 
;     number /= 10; 
;     places++; 
;   }
;   for (int i = 0; i < places; i++) {
;     System.out.print(s.pop());
;   }
; }
p       EQU 0                           ; Bindings for variables. 
places  EQU 4                           ; Bindings for variables. 
number  EQU 8                           ; Bindings for variables. 
i       EQU 12                          ; Bindings for variables. 
LCD_OutDec
        PUSH {R11, LR}                  ; R11 will be the stack frame pointer. 
        CMP R0, #0                      ; Handle the zero case. 
        BEQ zOd                         ; Handle the zero case. 
        SUB SP, #16                     ; Four local variables will be allocated via the stack. 
        MOV R11, SP                     ; R11 is now the stack frame register. 
        STR R0, [R11, #number]          ; Variable number = input argument. 
        MOV R0, #0                      ; Clear R0. 
        STR R0, [R11, #p]               ; Initialize variables. 
        STR R0, [R11, #places]          ; Initialize variables. 
        STR R0, [R11, #i]               ; Initialize variables. 
wLp     LDR R0, [R11, #number]          ; While number > 0
        MOV R1, #0                      ; While number > 0
        CMP R0, R1                      ; While number > 0
        BLS wLCont                      ; If Number <= 0, then continue. 
        BL Mod10                        ; p = number % 10 
        PUSH {R0,R3}                    ; s.push(p); 
        LDR R0, [R11, #number]          ; number /= 10; 
        MOV R1, #10                     ; number /= 10; 
        UDIV R0, R1                     ; number /= 10; 
        STR R0, [R11, #number]          ; number /= 10; 
        LDR R0, [R11, #places]          ; places++; 
        ADD R0, #1                      ; places++; 
        STR R0, [R11, #places]          ; places++; 
        B wLp                           ; While loop branch back. 
wLCont  NOP                             ; Continue. 
forlp   LDR R0, [R11, #i]               ; for (int i = 0; i < places; i++)
        LDR R1, [R11, #places]          ; for (int i = 0; i < places; i++)
        CMP R0, R1                      ; for (int i = 0; i < places; i++)
        BHS dn                          ; for (int i = 0; i < places; i++)
        POP {R0, R3}                    ; Print character (add 30 b/c ASCII) 
        ADD R0, #0x30                   ; Print character (add 30 b/c ASCII) 
        BL ST7735_OutChar               ; Print character (add 30 b/c ASCII) 
        LDR R0, [R11, #i]               ; for (int i = 0; i < places; i++)
        ADD R0, #1                      ; for (int i = 0; i < places; i++)
        STR R0, [R11, #i]               ; for (int i = 0; i < places; i++)
        B forlp                         ; for (int i = 0; i < places; i++)
dn      ADD SP, #16                     ; Deallocate variables. 
        POP {R11, PC}                   ; Restore R11 and return. 
zOd     MOV R0, #0x30                   ; Handle the zero case. 
        BL ST7735_OutChar               ; Handle the zero case. 
        POP {R11, PC}                   ; Handle the zero case. 

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
; Algorithm:
; static void OutFix(int number) {
;   if (number > 9999) {print("*.***"); return; }
;   Stack s = new Stack();
;   int p = 0;
;   int places = 4;
;   // Get the numbers. 
;   for (int i = 0; i < places; i++); 
;     p = number % 10;
;     s.push(p);
;     number /= 10;
;   }
;   for (int i = 0; i < places + 1; i++) {
;     if (i != 1)
;       System.out.print(s.pop());
;     else
;       System.out.print("."); 
;   }
; }
f        EQU 0                          ; Bindings for variables.
places2  EQU 4                          ; Bindings for variables.
number2  EQU 8                          ; Bindings for variables.
i2       EQU 12                         ; Bindings for variables.
maxN     EQU 9999                       ; Max number. 
LCD_OutFix
        PUSH {R11, LR}                  ; R11 will be the stack frame pointer.
        LDR R1, =maxN                   ; Test max. 
        CMP R0, R1                      ; Test for special case. 
        BHI SpeCs                       ; If it is greater than 9999, then output special case. 
        SUB SP, #16                     ; Four local variables will be allocated via the stack.
        MOV R11, SP                     ; R11 is now the stack frame register.
        STR R0, [R11, #number2]         ; Variable number = input argument.
        MOV R0, #0                      ; Clear R0.
        STR R0, [R11, #f]               ; Initialize variables.
        STR R0, [R11, #places2]         ; Initialize variables.
        MOV R0, #4                      ; Places is initialized to be 4. 
        STR R0, [R11, #places2]         ; Places is initialized to be 4. 
        MOV R0, #0                      ; For loop Initialize variables. 
        STR R0, [R11, #i2]              ; For loop Initialize variables.
forlp2  LDR R0, [R11, #i2]              ; for (int i = 0; i < places; i++)
        LDR R1, [R11, #places2]         ; for (int i = 0; i < places; i++)
        CMP R0, R1                      ; for (int i = 0; i < places; i++)
        BHS dn2                         ; for (int i = 0; i < places; i++)
        LDR R0, [R11, #number2]         ; Set R0 = number. 
        BL Mod10                        ; p = number % 10
        PUSH {R0,R3}                    ; s.push(p);
        LDR R0, [R11, #number2]         ; number /= 10;
        MOV R1, #10                     ; number /= 10;
        UDIV R0, R1                     ; number /= 10;
        STR R0, [R11, #number2]         ; number /= 10;
        LDR R0, [R11, #i2]              ; for (int i = 0; i < places; i++)
        ADD R0, #1                      ; for (int i = 0; i < places; i++)
        STR R0, [R11, #i2]              ; for (int i = 0; i < places; i++)
        B forlp2                        ; for (int i = 0; i < places; i++)
dn2     NOP                             ; Continue from for loop. 
        MOV R0, #0                      ; For loop Initialize variables. 
        STR R0, [R11, #i2]              ; For loop Initialize variables.
forlp3  LDR R0, [R11, #i2]              ; for (int i = 0; i < places + 1; i++)
        LDR R1, [R11, #places2]         ; for (int i = 0; i < places + 1; i++)
        ADD R1, #1                      ; This is the places + 1 part of the for loop. 
        CMP R0, R1                      ; for (int i = 0; i < places + 1; i++)
        BHS dn3                         ; for (int i = 0; i < places + 1; i++)
        LDR R0, [R11, #i2]              ; if (i != 1) 
        MOV R1, #1                      ; if (i != 1)
        CMP R0, R1                      ; if (i != 1)
        BNE pntN                        ; if (i == 1) then print ".". 
        MOV R0, #"."                    ; if (i == 1) then print ".". 
        BL ST7735_OutChar               ; if (i == 1) then print ".". 
        B conti                         ; Continue from if. 
pntN    NOP                             ; Print character if i != 1. 
        POP {R0, R3}                    ; Print character (add 30 b/c ASCII)
        ADD R0, #0x30                   ; Print character (add 30 b/c ASCII)
        BL ST7735_OutChar               ; Print character (add 30 b/c ASCII) 
conti   NOP                             ; Continue from if. 
        LDR R0, [R11, #i2]              ; for (int i = 0; i < places + 1; i++)
        ADD R0, #1                      ; for (int i = 0; i < places + 1; i++)
        STR R0, [R11, #i2]              ; for (int i = 0; i < places + 1; i++)
        B forlp3                        ; for (int i = 0; i < places + 1; i++)
dn3     NOP                             ; Continue from for loop. 
        ADD SP, #16                     ; Deallocate variables. 
        POP {R11, PC}                   ; Restore R11 and return. 
SpeCs   MOV R0, #"*"                    ; Output special case. 
        BL ST7735_OutChar               ; Output special case. 
        MOV R0, #"."                    ; Output special case. 
        BL ST7735_OutChar               ; Output special case. 
        MOV R0, #"*"                    ; Output special case. 
        BL ST7735_OutChar               ; Output special case. 
        MOV R0, #"*"                    ; Output special case. 
        BL ST7735_OutChar               ; Output special case. 
        MOV R0, #"*"                    ; Output special case. 
        BL ST7735_OutChar               ; Output special case. 
        POP {R11, PC}                   ; Restore R11 and return. 

    ALIGN
        
; -----------------------Mod10----------------------     
; Returns in R0 the modulo of 10. 
Mod10
    PUSH {R1, R2, R3, LR}
    MOV R1, #10
    UDIV R2, R0, R1
    MUL R2, R1
    SUB R0, R2
cot POP {R1, R2, R3, PC}

    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
