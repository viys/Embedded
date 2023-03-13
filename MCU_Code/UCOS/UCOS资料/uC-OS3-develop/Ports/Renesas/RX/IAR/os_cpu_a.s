;********************************************************************************************************
;                                              uC/OS-III
;                                        The Real-Time Kernel
;
;                    Copyright 2009-2022 Silicon Laboratories Inc. www.silabs.com
;
;                                 SPDX-License-Identifier: APACHE-2.0
;
;               This software is subject to an open source license and is distributed by
;                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
;                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
;
;********************************************************************************************************

;********************************************************************************************************
;
;                                       Renesas RX Specific Code
;
; File      : os_cpu_a.s
; Version   : V3.08.02
;********************************************************************************************************
; For       : Renesas RX
; Toolchain : IAR Embedded Workbench for Renesas RX
;********************************************************************************************************


;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************

    PUBLIC   _OSStartHighRdy             ; Public functions
    PUBLIC   _OSCtxSwISR                 ; Perform a context switch

    EXTERN   _OSTCBCurPtr                ; Declared as OS_TCB *               , 32-bit long
    EXTERN   _OSTCBHighRdyPtr            ; Declared as OS_TCB *               , 32-bit long
    EXTERN   _OSPrioCur                  ; Declared as INT8U                  ,  8-bit long
    EXTERN   _OSPrioHighRdy              ; Declared as INT8U                  ,  8-bit long
    EXTERN   _OSTaskSwHook
    EXTERN   _OS_KA_IPL_Boundary

;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

    section .text:CODE:ROOT


;*********************************************************************************************************
;                                         START MULTITASKING
;
; Description : This function is called by OSStart() to start the highest priority task that was created
;               by your application before calling OSStart().
;
; Arguments   : none
;
; Note(s)     : 1) The stack frame for the RX100 and RX200 families is assumed to look as follows:
;
;                                                                    LOW MEMORY
;                  OSTCBHighRdy->OSTCBStkPtr +  0  ---->  MACHI
;                                            +  4         MACMI
;                                            +  8         R1  (i.e. p_arg)     (32-bit contents of 'p_arg')
;                                            + 12         R2
;                                            + 16         R3
;                                            + 20         R4
;                                            + 24         R5
;                                            + 28         R6
;                                            + 32         R7
;                                            + 36         R8
;                                            + 40         R9
;                                            + 44         R10
;                                            + 48         R11
;                                            + 52         R12
;                                            + 56         R12
;                                            + 60         R14
;                                            + 64         R15
;                                            + 68         task                 (32-bit address of 'task')
;                                            + 72         PSW                  (Initial value of 0x0000)
;                                                                    HIGH MEMORY
;
;               2) The stack frame for the RX600 family is assumed to look as follows:
;
;                                                                    LOW MEMORY
;                  OSTCBHighRdy->OSTCBStkPtr +  0  ---->  MACHI
;                                            +  4         MACMI
;                                            +  8         FPSW
;                                            + 12         R1  (i.e. p_arg)     (32-bit contents of 'p_arg')
;                                            + 16         R2
;                                            + 20         R3
;                                            + 24         R4
;                                            + 28         R5
;                                            + 32         R6
;                                            + 36         R7
;                                            + 40         R8
;                                            + 44         R9
;                                            + 48         R10
;                                            + 52         R11
;                                            + 56         R12
;                                            + 60         R12
;                                            + 64         R14
;                                            + 68         R15
;                                            + 72         task                 (32-bit address of 'task')
;                                            + 76         PSW                  (Initial value of 0x0000)
;                                                                    HIGH MEMORY
;
;               3) OSStartHighRdy() MUST:
;                      a) Call OSTaskSwHook() then,
;                      b) Switch to the highest priority task.
;*********************************************************************************************************

_OSStartHighRdy:
    MOV.L   #_OSTaskSwHook, R5         ; Execute task switch hook
    JSR     R5

    SETPSW  U

    MOV.L   #_OSTCBHighRdyPtr, R5      ; SP = OSTCBHighRdyPtr->OSTCBStkPtr
    MOV.L   [R5], R2
    MOV.L   [R2], SP

                                       ; RESTORE CONTEXT OF NEW TASK
#if ((__VER__ >= 250) && (__CORE__ == 2))
    POPM    R1-R3                      ; Pop Accumulator registers
    MVTACLO R3, A0                     ; Write Accumulator 0
    MVTACHI R2, A0
    MVTACGU R1, A0
    POPM    R1-R3                      ; Pop Accumulator registers
    MVTACLO R3, A1                     ; Write Accumulator 1
    MVTACHI R2, A1
    MVTACGU R1, A1
#else
    POPM    R1-R2                      ; Pop Accumulator registers
    SHLL    #16, R2                    ; Correct for mid read/low write
    MVTACLO R2                         ; Write Accumulator
    MVTACHI R1
#endif

#if (((__VER__>=250) && (__FPU__==1)) || \
     ((__VER__<250) && (__CORE__!=__RX100__) && (__CORE__!=__RX200__)))
    POPC    FPSW                       ; Restore FP status word (NOT for RX100 and RX200 families)
#endif

    POPM    R1-R15                     ; Restore registers
    RTE                                ; Restore PSW and PC


;*********************************************************************************************************
;                                    PENDED CONTEXT SWITCH ISR
;
; Description : This ISR Performs all context switching within the OS.
;
; Arguments   : none
;
; Note(s)     : 1) Upon entry...
;                  OSTCBCur       Points to the OS_TCB of the task to suspend
;                  OSTCBHighRdy   Contains the pointer from the last scheduling
;
;
;               2) The interrupt stack frame looks as follows.  This was caused by the
;                  execution of SWINT event (the registers for the task to suspend need to be
;                  saved):
;
;                                                                         LOW MEMORY
;                                      ISP +  0  ---->  32-bit PC of task
;                                          +  4         32-bit PSW
;                                          +  8
;                                                                         HIGH MEMORY
;
;
;               3) The stack frame of the task to resume (for RX100 and RX200 families) looks as follows:
;
;                                                                         LOW MEMORY
;                  OSTCBHighRdyPtr->StkPtr +  0  ---->  MACHI
;                                          +  4         MACLO
;                                          +  8         R1 (p_arg)
;                                          + 12         R2
;                                          + 16         R3
;                                          + 20         R4
;                                          + 24         R5
;                                          + 28         R6
;                                          + 32         R7
;                                          + 36         R8
;                                          + 40         R9
;                                          + 44         R10
;                                          + 48         R11
;                                          + 52         R12
;                                          + 56         R12
;                                          + 60         R14
;                                          + 64         R15
;                                          + 68         task        (32-bit address of 'task')
;                                          + 72         PSW
;                                                                         HIGH MEMORY
;
;
;               4) The stack frame of the task to resume (for the RX600 family) looks as follows:
;
;                                                                         LOW MEMORY
;                  OSTCBHighRdyPtr->StkPtr +  0  ---->  MACHI
;                                          +  4         MACLO
;                                          +  8         FPSW
;                                          + 12         R1 (p_arg)
;                                          + 16         R2
;                                          + 20         R3
;                                          + 24         R4
;                                          + 28         R5
;                                          + 32         R6
;                                          + 36         R7
;                                          + 40         R8
;                                          + 44         R9
;                                          + 48         R10
;                                          + 52         R11
;                                          + 56         R12
;                                          + 60         R12
;                                          + 64         R14
;                                          + 68         R15
;                                          + 72         task        (32-bit address of 'task')
;                                          + 76         PSW
;                                                                         HIGH MEMORY
;*********************************************************************************************************

_OSCtxSwISR:
    SETPSW  I                          ; Re-enable higher priority interrupts.

                                       ; Transfer Interrupt Stack Contents to the current user stack
    PUSH.L  R15                        ; Save R15 to interrupt stack as we'll use for temporary register.
    MVFC    USP, R15                   ; Read the user stack pointer
    SUB     #12, R15                   ; Create space on user stack
    MVTC    R15, USP                   ; Update USP

                                       ; SAVE CONTEXT OF TASK BEING SWITCHED OUT
                                       ; Copy the data across from interrupt stack
    MOV.L   8[R0], 8[R15]              ; PSW
    MOV.L   4[R0], 4[R15]              ; PC
    MOV.L   0[R0], 0[R15]              ; R15
    ADD     #12, R0                    ; correct ISP for data we are done with using (RTE will be from USP)
    SETPSW  U                          ; Switch to user stack for pushing rest of registers
    PUSHM   R1-R14                     ; Save remaining registers
#if (((__VER__>=250) && (__FPU__==1)) || \
     ((__VER__<250) && (__CORE__!=__RX100__) && (__CORE__!=__RX200__)))
    PUSHC   FPSW                       ; Save FPSW (NOT for RX100 and RX200)
#endif

#if ((__VER__ >= 250) && (__CORE__ == 2))
    MVFACGU #0, A1, R1                 ; Save special registers
    MVFACHI #0, A1, R2
    MVFACLO #0, A1, R3
    PUSHM   R1-R3
    MVFACGU #0, A0, R1
    MVFACHI #0, A0, R2
    MVFACLO #0, A0, R3
    PUSHM   R1-R3
#else
    MVFACHI R1
    MVFACMI R2
    PUSHM   R1-R2
#endif

                                       ; Context is saved, registers can be used, handle OS requirements
    MOV.L   #_OS_KA_IPL_Boundary, R2   ; Set IPL to Kernel Awareness boundary
    MOV.L   [R2], R1
    MVFC    PSW,  R2
    AND     #-0F000001H, R2
    OR      R1,   R2
    MVTC    R2,   PSW

    MOV.L   #_OSTCBCurPtr, R6          ; Save current task's SP into its TCB
    MOV.L   [R6], R2
    MOV.L   SP, [R2]

    MOV.L   #_OSTaskSwHook, R5         ; Execute task switch hook
    JSR     R5

    MOV.L   #_OSPrioCur, R3            ; OSPrioCur = OSPrioHighRdy
    MOV.L   #_OSPrioHighRdy, R2
    MOV.B   [R2], [R3]

    MOV.L   #_OSTCBHighRdyPtr, R2      ; OSTCBCurPtr = OSTCBHighRdyPtr
    MOV.L   [R2], [R6]
    MOV.L   [R6], R2
    MOV.L   [R2], SP                   ; SP = OSTCBCurPtr->StkPtr

    MVTIPL  #1                         ; Restore IPL to level 1

                                       ; RESTORE CONTEXT OF NEW TASK
#if ((__VER__ >= 250) && (__CORE__ == 2))
    POPM    R1-R3                      ; Pop Accumulator registers
    MVTACLO R3, A0                     ; Write Accumulator 0
    MVTACHI R2, A0
    MVTACGU R1, A0
    POPM    R1-R3                      ; Pop Accumulator registers
    MVTACLO R3, A1                     ; Write Accumulator 1
    MVTACHI R2, A1
    MVTACGU R1, A1
#else
    POPM    R1-R2                      ; Pop Accumulator registers
    SHLL    #16, R2                    ; Correct for mid read/low write
    MVTACLO R2                         ; Write Accumulator
    MVTACHI R1
#endif

#if (((__VER__>=250) && (__FPU__==1)) || \
     ((__VER__<250) && (__CORE__!=__RX100__) && (__CORE__!=__RX200__)))
    POPC    FPSW                       ; Restore FPSW (NOT for RX100 and RX200)
#endif
    POPM    R1-R15
    RTE

    END
