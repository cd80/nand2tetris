// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

	// initialize variables
	// @size = 8191(from 0)
	@8191
	D=A
	@size
	M=D
	// @idx = 0
	@idx
	M=0
	// @fill = 0(/-1)
	@fill
	M=0
	// @RET used when calling (FILL_SCREEN)
	@RET
	M=0

	// initialize program
	@KEY_NOT_PRESSED
	0;JMP

(FILL_SCREEN)
	// set @idx to @size
	@size
	D=M
	@idx
	M=D

	(LOOP)
	// pointer = @SCREEN + idx
	@SCREEN
	D=A
	@pointer
	M=D
	@idx
	D=M
	@pointer
	M=M+D

	// fill pointer with @fill
	@fill
	D=M
	@pointer
	A=M
	M=D

	// decrease idx and goto LOOP
	// jmp to LOOPEND if idx+1 == 0
	@idx
	MD=M-1
	@LOOPEND
	D+1;JEQ

	@LOOP
	0;JMP

	(LOOPEND)

	@RET
	A=M
	0;JMP

(KEY_NOT_PRESSED)
	// clear SCREEN and loop while @KBD != 0
	
	// clear SCREEN
	@fill
	M=0

	// set RET
	@KNP_AFTERFILL
	D=A
	@RET
	M=D

	// call FILL_SCREEN(will return to KMP_AFTERFILL)
	@FILL_SCREEN
	0;JMP

	(KNP_AFTERFILL)
	@KBD
	D=M
	@KEY_PRESSED
	D;JNE
	@KNP_AFTERFILL
	0;JMP
	// loop and check @KBD != 0, if KBD != 0, goto KEY_PRESSED


(KEY_PRESSED)
	// fill SCREEN and loop while @KBD == 0

	// fill SCREEN
	@fill
	M=-1

	@KP_AFTERFILL
	D=A
	@RET
	M=D


	// call FILL_SCREEN(will return to KP_AFTERFILL)
	@FILL_SCREEN
	0;JMP

	(KP_AFTERFILL)
	// loop and check @KBD == 0, if KBD == 0, goto KEY_NOT_PRESSED
	@KBD
	D=M
	@KEY_NOT_PRESSED
	D;JEQ
	@KP_AFTERFILL
	0;JMP
