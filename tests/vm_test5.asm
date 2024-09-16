	# $Id: vm_test5.asm,v 1.4 2024/08/29 20:26:00 leavens Exp $
	.text start
start:	ADDI $sp, 0, 0x7  # stack top is 7
	ADDI $sp, -1, 0xf # 1 above stack top is 15
	ARI $sp, -1       # make the stack top be the location with 15
	LWR $r4, $gp, 0    # $r4 should be 2
	AND $sp, 0, $sp, 0 # stack top should (still) be 15
	SLL  $sp, 0, 5    # stack top should be 480
	ADDI $sp, 0, -405 # stack top is 75
        SRI $sp, 1        # allocate a word on the stack
	ADDI $sp, 0, 2    # put 2 on top of stack
	SLL $sp, 0, 7     # top of stack is 256
	SRL $sp, 0, 2     # stack top is 64
        ARI $sp, -1       # allocate a word on the stack
	LIT $sp, 0, 11    # put 11 on top of stack
        LWR $r5, $sp, 0   # put 11 in $r5
	XOR $sp, 0, $sp, 0 # make top of stack be 0
	SWR $sp, 0, $r5   # put 11 on top of stack
	NEG $sp, 0, $sp, 0 # make top of stack be -11
        LIT $sp, 1, 75    # put 75 in memory[SP+1]
        ARI $sp, 1        # pop the stack
	BNE $sp, 1, 3     # skip if top 2 stack elements are not equal
        CALL y
	JMPA done
	CALL n
	JMPA bad
done:	EXIT 0
bad:	EXIT 1

y:	NOTR
	PSTR $gp, 5
	STRA
	RTN

n:	NOTR
	PSTR $gp, 1
	STRA
	RTN

	.data 512
	WORD TWO = 2
	STRING[1] No = "No\n"
	WORD L = 76
	WORD H = 72
	WORD nl = 10
	STRING[2] Yes = "Yes!\n"
	.stack 4096
	.end
	
