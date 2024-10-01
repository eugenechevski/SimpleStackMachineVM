	# $Id: vm_testC.asm,v 1.1 2024/09/25 14:16:06 leavens Exp $
	.text 0
	SRI $sp, 1          # allocate a word on stack
	LIT $sp, 0, 1       # put a 1 on top of the stack
	SLL $sp, 0, 30      # make top of stack be 2^30
	SUB $sp, 0, $gp, 0  # make top of stack be (2^30)-1
	BEQ $gp, 7, +2
	JMPA wrong
	MUL $sp, 0          # (HI,LO) is ((2^30)-1)^2
	CFHI $sp, 0         # HI is most significant bits of result
	BEQ $gp, 8, +2
	JMPA wrong
	CFLO $sp, 0
	BEQ $gp, 9, +2
	JMPA wrong
	NOTR
pass:	PSTR $gp, 4
	EXIT 0
wrong:	PSTR $gp, 1
	EXIT 1
	.data 4096
	WORD one = 1
	STRING[3] Wrong = "Wrong!\n"
	STRING[3] Passed = "Passed!\n"
	WORD two30m1 = 1073741823  # (2^30)-1
	WORD mostSignif = 268435455
	WORD leastSignif = -2147483647
	.stack 32000
	.end
