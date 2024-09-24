	# $Id: vm_test7.asm,v 1.5 2024/08/15 18:39:21 leavens Exp $
	# manipulation of the runtime stack
	.text main
main:   SRI $sp, 6       # put numbers in the registers
	LIT $sp, 6, 7
	LWR $ra, $sp, 6
	LIT $sp, 5, 6
	LWR $r6, $sp, 5
	LIT $sp, 4, 5
	LWR $r5, $sp, 4
	LIT $sp, 3, 4
	LWR $r4, $sp, 3
	LIT $sp, 2, 3
	LWR $r3, $sp, 2
	LIT $sp, 1, 2
	#  LWR $sp, $sp, 1   # putting 2 in $sp would violate a machine invariant
	SCA $sp, 0, $sp, 0
	
	CALL callerSave
	SRI $sp, 1
	LIT $sp, 0, -1
	LWR $r6, $sp, 0
	LWR $r5, $sp, 0
	LWR $r4, $sp, 0
	LWR $r3, $sp, 0
	ARI $sp, 1
	CALL callerRestore
	
	EXIT 0
bad:	EXIT 1
	
	# callerSave is a subroutine that saves the registers
	# that a caller might need to save on the stack.
	# These registers are left undistubed...
	# However, since this is a subroutine, it can't save $ra,
	# and it disturbs $sp, so those would need to be saved first
callerSave: SRI $sp, 5        # allocate 5 words on the stack
	SWR $sp, 4, $fp   # save $fp...
	SWR $sp, 3, $r3
	SWR $sp, 2, $r4
	SWR $sp, 1, $r5
	SWR $sp, 0, $r6
        RTN
	
	
	# callerRestore is a subroutine that restores all registers
	# that were saved by callerSave (on the stack)
	# Assuming the runtime stack is in the same state it was
	# after calling callerSave, it also restores the $sp
	# to its previous value.
	# Note that the user will need to restore $ra manually
	# after calling this subroutine.
callerRestore: LWR $r6, $sp, 0
	LWR $r5, $sp, 1  # restore $r5 ...
	LWR $r4, $sp, 2
	LWR $r3, $sp, 3
	LWR $fp, $sp, 4
	ARI $sp, 5       # deallocate the 5 words
        RTN

	.data 1024
	.stack 4096
	.end
