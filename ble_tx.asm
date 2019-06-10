
; *****************************************************************************
; File:     ble_tx.asm
; Date:
;
; *****************************************************************************

    .cdecls C, LIST, "blebs_base.h"
    .include "NOPdefs.asm"
    .def sendBLEPacket
    .def TA_sendBLEPacket

R_TxDelay   .set    R9
R_bitout    .set    R10
R_prevState .set    R11
R_currBit	.set    R12
R_currByte	.set	R13
R_dataPtr 	.set	R14
R_byteCt    .set  	R15



sendBLEPacket:
    ;setup ble_tx register
    MOV	      &(ptr_blePacket), R_dataPtr
    MOV.B     &(len_blePacket), R_byteCt
    MOV.B     #BIT7,  R_bitout            ;data in P1.7, LSB

    ;turn on oscillator
    BIS       #BIT6,  &(P1OUT)
    BIS       #BIT5,  &(P1OUT)
    BIS       #BIT0,  &(P3OUT)
	MOV		  #TX_DELAY2,  R_TxDelay

	MOV.B     &(P1OUT),  R_prevState
    AND       #0x7F, R_prevState
    ;MOV      #0 , R_prevState
	;JMP       LoadByte

TxWakeupDelay:
	NOP								 ;[1]
	DEC		  R_TxDelay 			 ;[1]
	JNZ		  TxWakeupDelay			 ;[2]

TimingCompensate:
    JMP       LoadByte               ;[1.5]

LoadByte:
    MOV.B     @R_dataPtr+, R_currByte;[2]

SendByte:
    ;b0
    MOV.B     R_bitout,   R_currBit  ;[1]
    AND       R_currByte, R_currBit  ;[1]
    BIS       R_prevState,R_currBit  ;[1]
    MOV.B     R_currBit,  &(P1OUT)   ;[4]
    RLA       R_currByte             ;[1]
    NOPx6                            ;[6]

    ;b1
    MOV.B     R_bitout,   R_currBit  ;[1]
    AND       R_currByte, R_currBit  ;[1]
    BIS       R_prevState,R_currBit  ;[1]
    MOV.B     R_currBit,  &(P1OUT)   ;[4]
    RLA       R_currByte             ;[1]
    NOPx6                            ;[6]

    ;b2
    MOV.B     R_bitout,   R_currBit  ;[1]
    AND       R_currByte, R_currBit  ;[1]
    BIS       R_prevState,R_currBit  ;[1]
    MOV.B     R_currBit,  &(P1OUT)   ;[4]
    RLA       R_currByte             ;[1]
    NOPx6                            ;[6]

    ;b3
    MOV.B     R_bitout,   R_currBit  ;[1]
    AND       R_currByte, R_currBit  ;[1]
    BIS       R_prevState,R_currBit  ;[1]
    MOV.B     R_currBit,  &(P1OUT)   ;[4]
    RLA       R_currByte             ;[1]
    NOPx6                            ;[6]

    ;b4
    MOV.B     R_bitout,   R_currBit  ;[1]
    AND       R_currByte, R_currBit  ;[1]
    BIS       R_prevState,R_currBit  ;[1]
    MOV.B     R_currBit,  &(P1OUT)   ;[4]
    RLA       R_currByte             ;[1]
    NOPx6                            ;[6]

    ;b5
    MOV.B     R_bitout,   R_currBit  ;[1]
    AND       R_currByte, R_currBit  ;[1]
    BIS       R_prevState,R_currBit  ;[1]
    MOV.B     R_currBit,  &(P1OUT)   ;[4]
    RLA       R_currByte             ;[1]
    NOPx6                            ;[6]

    ;b6
    MOV.B     R_bitout,   R_currBit  ;[1]
    AND       R_currByte, R_currBit  ;[1]
    BIS       R_prevState,R_currBit  ;[1]
    MOV.B     R_currBit,  &(P1OUT)   ;[4]
    RLA       R_currByte             ;[1]
    NOPx6                            ;[6]

    ;b7
    MOV.B     R_bitout,   R_currBit  ;[1]
    AND       R_currByte, R_currBit  ;[1]
    BIS       R_prevState,R_currBit  ;[1]
    MOV.B     R_currBit,  &(P1OUT)   ;[4]

    DEC       R_byteCt               ;[1]
    NOP
    NOP
    JNZ       LoadByte       ;[1.5]

TxTurnOff:
    NOPx3                            ;[3]
    MOV.B     #0,         R_currBit  ;[2]
    BIS       R_prevState,R_currBit  ;[1]
    MOV.B     R_currBit,  &(P1OUT)   ;[4]

    MOV		  #TX_DELAY,  R_TxDelay  ;

TxTurnOffDelay:
	NOP								 ;[1]
	DEC		  R_TxDelay  			 ;[1]
	JNZ		  TxTurnOffDelay		 ;[2]
	BIC       #BIT5,      &(P1OUT)   ;
	BIC       #BIT0,  &(P3OUT)
	BIC       #BIT6,      &(P1OUT)   ;

    RETA


