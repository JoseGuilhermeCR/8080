# 8080
In this repository, you will find an Intel 8080 emulator, a disassembler and an assembler.

## Emulator
The emulator itself is in i8080.c. You can execute your own binary, execute the cpu tests
or execute an interrupt test, more like a demo, I made.
I decided to use IO PORT 0 to output characters. So, if you make your own program, by using "out 0"
you can output a char to the screen.
Example:
```
[joseguilhermecr@jose-desktop 8080]$ ./i8080 assembler/file.hex
Starting with: start_pc = 0x100 memory_offset = 0x100 debug = 0 step_by_step = 0
Hello There, 8080!
```
## Assembler and Disassembler
The disassembler was made before the actual emulator, so I could get familiar with the instructions.
Be aware that the disassembler can't differentiate instructions from data. So, a really weird
sequence of instructions may actually be data that was interpreted as instructions.

The assembler was made in Rust and is mostly functional. Assembler commands include:
``.org`` to specify the origin of the program in memory.
``.db`` to define a byte.
``.dw`` to define a word.
``.asciiz`` to define a null terminated string.

You can assemble code by running:
```assembler "file.asm" --output "optional_output_name.hex"```.
If you don't specify the name of the output file, it will be called file.hex.

## Tests

CPU tests were made to be run within CP/M, an ancient OS, and, therefore, they made use of a BIOS.
I put a return instruction at the address where the BIOS function should be and just checked whether 
the tests had jumped to that address so I could print the information they wanted to show.
That's how the tests are able to print to the screen.

Output when running i8080 cpu_tests:
```
=====================================================
Loaded file cpu/tests/TEST.COM with size 1793 and offset 256.
MICROCOSM ASSOCIATES 8080/8085 CPU DIAGNOSTIC VERSION 1.0  (C) 1980

CPU IS OPERATIONAL
Got to 0000, PC before = 14FH 335
Done instructions: 2148. Done cycles: 19281
=====================================================

=====================================================
Loaded file cpu/tests/8080PRE.COM with size 1024 and offset 256.
8080 Preliminary tests complete
Got to 0000, PC before = 32FH 815
Done instructions: 1059. Done cycles: 7797
=====================================================

=====================================================
Loaded file cpu/tests/8080EXM.COM with size 4608 and offset 256.
8080 instruction exerciser
dad <b,d,h,sp>................  PASS! crc is:14474ba6
aluop nn......................  PASS! crc is:9e922f9e
aluop <b,c,d,e,h,l,m,a>.......  PASS! crc is:cf762c86
<daa,cma,stc,cmc>.............  PASS! crc is:bb3f030c
<inr,dcr> a...................  PASS! crc is:adb6460e
<inr,dcr> b...................  PASS! crc is:83ed1345
<inx,dcx> b...................  PASS! crc is:f79287cd
<inr,dcr> c...................  PASS! crc is:e5f6721b
<inr,dcr> d...................  PASS! crc is:15b5579a
<inx,dcx> d...................  PASS! crc is:7f4e2501
<inr,dcr> e...................  PASS! crc is:cf2ab396
<inr,dcr> h...................  PASS! crc is:12b2952c
<inx,dcx> h...................  PASS! crc is:9f2b23c0
<inr,dcr> l...................  PASS! crc is:ff57d356
<inr,dcr> m...................  PASS! crc is:92e963bd
<inx,dcx> sp..................  PASS! crc is:d5702fab
lhld nnnn.....................  PASS! crc is:a9c3d5cb
shld nnnn.....................  PASS! crc is:e8864f26
lxi <b,d,h,sp>,nnnn...........  PASS! crc is:fcf46e12
ldax <b,d>....................  PASS! crc is:2b821d5f
mvi <b,c,d,e,h,l,m,a>,nn......  PASS! crc is:eaa72044
mov <bcdehla>,<bcdehla>.......  PASS! crc is:10b58cee
sta nnnn / lda nnnn...........  PASS! crc is:ed57af72
<rlc,rrc,ral,rar>.............  PASS! crc is:e0d89235
stax <b,d>....................  PASS! crc is:2b0471e9
Tests complete
Got to 0000, PC before = 137H 311
Done instructions: 2919050420. Done cycles: 23835662278
=====================================================

=====================================================
Loaded file cpu/tests/CPUTEST.COM with size 19200 and offset 256.

DIAGNOSTICS II V1.2 - CPU TEST
COPYRIGHT (C) 1981 - SUPERSOFT ASSOCIATES

ABCDEFGHIJKLMNOPQRSTUVWXYZ
CPU IS 8080/8085
BEGIN TIMING TEST
END TIMING TEST
CPU TESTS OK

Got to 0000, PC before = 3B25H 15141
Done instructions: 33971128. Done cycles: 255663225
=====================================================
```
