	# $Id: vm_test3.asm,v 1.5 2024/08/29 20:26:00 leavens Exp $
	.text start
start:	STRA
	ADDI $sp, 0, 30   # stack top is 30
	SRI $sp, 1        # subtract 1 from SP (allocate a word)
	ADDI $sp, 0, 20   # top of stack is 20
	ADDI $sp, 0, 10   # stack top is 30
	BNE $sp, 1, +3    # skip if stack top and stack+1 are not equal
	CALL yes
	JMPA done
	CALL no
	JMPA done
yes:    NOTR
	PSTR $gp, 1 # print yes
	RTN
no:	NOTR
	PSTR $gp, 2 # print no
	RTN
	STRA
	EXIT 1
done:	STRA
	EXIT 0
	
	.data 1024
	WORD x = 33
	STRING[1] yeah = "Y\n"
	STRING[1] nay = "No\n"
	.stack 4096
	.end
	
