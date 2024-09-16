	# $Id: vm_test6.asm,v 1.7 2024/08/29 20:26:00 leavens Exp $
	.text start
start:	ADDI $sp, -1, 1	  # one word above top of stack gets 1
	CPW $sp, 0, $sp, -1
	SLL $sp, -1, 20   # one word above top of stack gets 2^20
	SRI $sp, 1        # SP = SP - 1
	ADD $sp, 0, $gp, 0 # top of stack is 2^20+1
	SRI $sp, 1        # allocate a word
	LIT $sp, 0, 4     # put 4 on top of stack
	LWR $r4, $sp, 0   # $r4 is 4
	LIT $sp, 0, 14    # top of stack is 14
	LWR $r5, $sp, 0   # $r5 is 14
	SWR $sp, -1, $r4  # one word above top of stack gets 4
        ARI $sp, -2       # allocate 2 words on the stack
	SWR $sp, 0, $r5   # top of stack gets 14
	DIV $sp, 1        # HI is 2, LO is 3
	CFLO $gp, 2       # dividend holds 3
	CFHI $gp, 1       # rem is 2
	SRI $sp, 1        # allocate a word on top of stack
	LIT $sp, 0, 1     # top of stack gets 1
	SLL $sp, 0, 10    # put 2^10 (i.e., 1024) on top of stack
	MUL $sp, 0        # square top of stack, result in (HI, LO)
	CFHI $sp, -2
	CFLO $sp, -1
	SRI $sp, 2
	EXIT 0
	.data 1024
	WORD one = 1
	WORD rem = 0
	WORD dividend = 0
	.stack 4096
	.end
