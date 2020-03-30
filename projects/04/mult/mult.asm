// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Put your code here.

// set R2 = 0
@0
D=A
@R2
M=D

// store R1 somewhere else(@times)
@R1
D=M
@times
M=D

// loop while @times == 0, @R2 = @R2 + @R0
(LOOP)
	// if @times == 0: goto LOOPEND
	@times
	D=M
	@LOOPEND
	D;JEQ

	// if @R0 == 0: goto LOOPEND
	@R0
	D=M
	@LOOPEND
	D;JEQ

	@R0
	D=M
	@R2
	M=M+D

	@LOOPEND
	D;JEQ

	@times
	M=M-1
	@LOOP
	0;JMP

(LOOPEND)
	@LOOPEND
	0;JMP