# $Id: vm_test4.asm,v 1.5 2024/08/29 20:26:00 leavens Exp $
	.text start
start:	NOTR
	STRA
	ADDI $sp, 0, 5     # stack top is 5
	ADDI $sp, -1, 2    # memory[SP-1] is 2
	ARI $sp, -1        # make stack top point to where the 2 is
	ADD $sp, 0, $sp, 1 # stack top should be 7
	MUL $sp, 1         # LO should be (5 * 7) = 35
	SRI $sp, 1         # allocate a word on stack
	CFLO $sp, 0        # stack top is 35
	SUB $sp, 0, $sp, 1 # stack top should be (35-7) = 28
	MUL $sp, 2         # LO should be (28 * 5) = 140
        SRI $sp, 1         # allocate a word on stack
	CFLO $sp, 0        # stack top is 140
	SRI $sp, 1         # allocate a word on stack
	CPW $sp, 0, $sp, 2 # put 28 on top of stack
	DIV $sp, 4	   # HI should be (28 % 5) = 3, LO s.b. (28 / 5) = 5
        SRI $sp, 2         # allocate two words on stack
	CFLO $sp, 0        # stack top should be 5
	CFHI $sp, 1        # second from top should be 3
	SRI $sp, 1         # allocate a word on the stack
	ADDI $sp, 0, 5     # store 5! on top of stack
	BEQ $sp, 1, 2      # write no and exit 1 if memory[$sp+1] is not 5
	CALL no
	CALL yes
	ARI $sp, 1         # pop a word off the stack
	SWR $sp, 0, $r4    # store a 0 (from $r4) into top of stack
	ADDI $sp, 0, 3     # make top of stack be 3
	BEQ $sp, 1, 2      # write no if memory[$sp+1] is not 3
	CALL no
	CALL yes
done:	EXIT 0

yes:	NOTR
	PSTR $gp, 0       # print "y\n"
	STRA
	RTN

no:	NOTR
	PSTR $gp, 1       # print "no\n"
	STRA
	EXIT 1
        RTN
	
	.data 512
	STRING[1] y = "y\n"
	STRING[1] n = "no\n"
	.stack 4096
	.end
	
