;
; @file     examples/lexer/keywords.asm
; @brief    Contains tokens to be tested only by the TM32 Assembler Tool's
;           Lexer Component
;

; NOTE: Run this with `tm32asm --lex-only examples/lexer/keywords.asm`.

; =====================================================================
; PREPROCESSOR DIRECTIVES
; =====================================================================

; File inclusion and macro definitions
.include
.INCLUDE
.Include

.let
.LET
.Let

.const
.CONST
.Const

.define
.DEFINE
.Define

.def
.DEF
.Def

.macro
.MACRO
.Macro

.shift
.SHIFT
.Shift

.endmacro
.ENDMACRO
.Endmacro

.endm
.ENDM
.Endm

; Conditional assembly
.if
.IF
.If

.else
.ELSE
.Else

.elseif
.ELSEIF
.Elseif

.elif
.ELIF
.Elif

.endif
.ENDIF
.Endif

.endc
.ENDC
.Endc

; Repetition and loops
.repeat
.REPEAT
.Repeat

.rept
.REPT
.Rept

.endrepeat
.ENDREPEAT
.Endrepeat

.endr
.ENDR
.Endr

.while
.WHILE
.While

.endwhile
.ENDWHILE
.Endwhile

.endw
.ENDW
.Endw

.for
.FOR
.For

.endfor
.ENDFOR
.Endfor

.endf
.ENDF
.Endf

.continue
.CONTINUE
.Continue

.break
.BREAK
.Break

; Error handling and debugging
.warn
.WARN
.Warn

.error
.ERROR
.Error

.assert
.ASSERT
.Assert

.file
.FILE
.File

.line
.LINE
.Line

; =====================================================================
; ASSEMBLER DIRECTIVES
; =====================================================================

; Section definitions
.metadata
.METADATA
.Metadata

.interrupt
.INTERRUPT
.Interrupt

.int
.INT
.Int

.text
.TEXT
.Text

.code
.CODE
.Code

.rodata
.RODATA
.Rodata

.data
.DATA
.Data

.bss
.BSS
.Bss

.org
.ORG
.Org

; Data definitions
.db
.DB
.Db

.byte
.BYTE
.Byte

.dw
.DW
.Dw

.word
.WORD
.Word

.dd
.DD
.Dd

.dword
.DWORD
.Dword

.asciz
.ASCIZ
.Asciz

.stringz
.STRINGZ
.Stringz

.align
.ALIGN
.Align

.ds
.DS
.Ds

.space
.SPACE
.Space

.incbin
.INCBIN
.Incbin

; =====================================================================
; BUILT-IN FUNCTIONS
; =====================================================================

; Bit manipulation functions
high
HIGH
High

low
LOW
Low

bitwidth
BITWIDTH
Bitwidth

; Math functions
integer
INTEGER
Integer

fraction
FRACTION
Fraction

round
ROUND
Round

ceil
CEIL
Ceil

floor
FLOOR
Floor

; Trigonometric functions
sin
SIN
Sin

cos
COS
Cos

tan
TAN
Tan

asin
ASIN
Asin

acos
ACOS
Acos

atan
ATAN
Atan

; String functions
strlen
STRLEN
Strlen

strupper
STRUPPER
Strupper

strlower
STRLOWER
Strlower

strcmp
STRCMP
Strcmp

strfind
STRFIND
Strfind

strrfind
STRRFIND
Strrfind

strbyte
STRBYTE
Strbyte

; Symbol and section functions
defined
DEFINED
Defined

isconst
ISCONST
Isconst

isvariable
ISVARIABLE
Isvariable

section
SECTION
Section

origin
ORIGIN
Origin

sizeof
SIZEOF
Sizeof

startof
STARTOF
Startof

; =====================================================================
; GENERAL-PURPOSE CPU REGISTERS
; =====================================================================

; 32-bit registers
a
A

b
B

c
C

d
D

; 16-bit registers
aw
AW
Aw

bw
BW
Bw

cw
CW
Cw

dw
DW
Dw

; 8-bit high registers
ah
AH
Ah

bh
BH
Bh

ch
CH
Ch

dh
DH
Dh

; 8-bit low registers
al
AL
Al

bl
BL
Bl

cl
CL
Cl

dl
DL
Dl

; =====================================================================
; EXECUTION CONDITIONS
; =====================================================================

nc
NC
Nc

zc
ZC
Zc

zs
ZS
Zs

cc
CC
Cc

cs
CS
Cs

pc
PC
Pc

ps
PS
Ps

sc
SC
Sc

ss
SS
Ss

; =====================================================================
; CPU INSTRUCTIONS
; =====================================================================

; System control instructions
nop
NOP
Nop

stop
STOP
Stop

halt
HALT
Halt

sec
SEC
Sec

cec
CEC
Cec

di
DI
Di

ei
EI
Ei

daa
DAA
Daa

scf
SCF
Scf

ccf
CCF
Ccf

flg
FLG
Flg

stf
STF
Stf

clf
CLF
Clf

; Data movement instructions
ld
LD
Ld

ldh
LDH
Ldh

ldp
LDP
Ldp

st
ST
St

sth
STH
Sth

stp
STP
Stp

mv
MV
Mv

pop
POP
Pop

push
PUSH
Push

; Control flow instructions
jmp
JMP
Jmp

jpb
JPB
Jpb

call
CALL
Call

int
INT
Int

ret
RET
Ret

reti
RETI
Reti

; Arithmetic instructions
inc
INC
Inc

dec
DEC
Dec

add
ADD
Add

adc
ADC
Adc

sub
SUB
Sub

sbc
SBC
Sbc

; Logical instructions
and
AND
And

or
OR
Or

xor
XOR
Xor

not
NOT
Not

cmp
CMP
Cmp

; Shift and rotate instructions
sla
SLA
Sla

sra
SRA
Sra

srl
SRL
Srl

rl
RL
Rl

rlc
RLC
Rlc

rr
RR
Rr

rrc
RRC
Rrc

; Bit manipulation instructions
bit
BIT
Bit

tog
TOG
Tog

set
SET
Set

res
RES
Res