	# $Id: vm_test1.asm,v 1.6 2024/07/29 02:01:32 leavens Exp $
	.text start
start:	STRA
	SRI $sp, 2         # allocate 2 words on the stack
	ADDI $sp, 1, 1     # put 1 in memory[$sp+1]
	ADD $gp, 2, $sp, 1 # put 1 in temp
	ADD $sp, 0, $gp, 2 # put 1 in stack top
	SUB $gp, 2, $gp, 2 # zero out temp
	ADDI $sp, 0, 10    # add 10 to stack top, should be 11
	SRI $sp, 1         # allocate a word on the stack
	NOTR
	PCH $gp, 0         # print y
	PCH $gp, 3         # print nl
	EXIT 0
	.data 1024
	CHAR y = 'y'
	CHAR n = 'n'
	WORD temp    # not initialized
	CHAR nl = '\n'
	.stack 4096
	.end
	
