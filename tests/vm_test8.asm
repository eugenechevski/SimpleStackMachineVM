	# $Id: vm_test8.asm,v 1.4 2024/08/15 18:58:50 leavens Exp $
	.text 0
	LIT $sp, 0, 17
	LWR $r4, $sp, 0   # put 14 in $r4
	SRI $sp, 2
	LIT $sp, 0, 19
	LWR $r5, $sp, 0   # put 19 in $r5
	SCA $sp, -1, $sp, 0 # store stack top address
	LWI $sp, -2, $sp, -1
	SRI $sp, 2
	EXIT 0
	.data 256
	.stack 1024
	.end
