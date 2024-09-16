	# $Id: vm_test9.asm,v 1.2 2024/08/22 03:03:33 leavens Exp $
	.text 0
	SRI $sp, 1
	XORI $sp, 0, 0x567a
	LIT $sp, -1, 0x67a
	BORI $sp, -1, 0x5000
	BNE $sp, -1, 8 # fail if not equal
	ANDI $sp, 0, 0xFFF
	LIT $sp, -2, 0x67a
	BNE $sp, -2, 5 # fail if not equal
good:   NOTR
	PSTR $gp, 0
	STRA
	EXIT 0
bad:	NOTR
	PSTR $gp, 1
	STRA
	EXIT 1
	.data 1024
	STRING[1] Y = "Y\n"
	STRING [1] no = "no\n"
	CHAR nl = '\n'
	.stack 4096
	.end
