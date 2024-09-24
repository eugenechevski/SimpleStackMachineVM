	# $Id: vm_test0.asm,v 1.3 2024/07/26 12:44:46 leavens Exp $
	.text start
start:	STRA
	ADDI $sp, -1, 2
	EXIT 0
	.data 1024
	.stack 4096
	.end
	
