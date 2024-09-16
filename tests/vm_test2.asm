	# $Id: vm_test2.asm,v 1.3 2024/07/29 02:01:32 leavens Exp $
	.text start
start:	ADDI $sp, 0, 1    # top of stack is 1
	ADD $sp, 0, $sp, 0 # top of stack is 2
	ADDI $sp, 0, 1    # top of stack is 3
	ADD $sp, 0, $sp, 0 # $top of stack is 6
	SLL $sp, 0, 1     # top of stack is 12
	SRI $sp, 4        # allocate a word on the stack
	# NOTR
	PCH $gp, 1        # print 'Y'
	PCH $gp, 2        # print '\n'
	EXIT 0
	.data 1024
	WORD x = 33
	CHAR y = 'Y'
	CHAR nl = '\n'
	.stack 4096
	.end
	
