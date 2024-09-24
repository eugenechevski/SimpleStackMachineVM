	# $Id: vm_testA.asm,v 1.1 2024/08/22 13:33:49 leavens Exp $
	.text 0
	SRI $sp, 1
	XORI $sp, 0, 0xffff
	LIT $sp, -1, 0xff
	BORI $sp, -1, 0xff00
	BNE $sp, -1, 5 # fail if not equal
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
