bin:	sub	$sp, $sp, $imm, 4						#adjust stack for 4 items	(every enter is 4 byte size so only need to sub 4 places for 4 items)
		sw $ra, $sp, $imm, 0						# save ra in 0($sp)
		sw $a0, $sp, $imm, 1						# save a0 in 1($sp)
		sw $a1, $sp, $imm, 2						# save a1 in 2($sp)
		sw $s0, $sp, $imm, 3						# save s0 in 3($sp)
 
		add $v0, $zero, $imm, 1						# set vo = 1
		beq $ra, $a1, $zero, 0						# if(k==0) , return (v0 allready set to 1)
		beq $ra, $a1, $a0, 0						# if(k==n) , return 

		sub $a0, $a0, $imm, 1						# n=n-1  (preparing for new bin calculate)
		sub $a1, $a1, $imm, 1						# k=k-1 
		jal $imm, $zero, $zero, bin					#  recursive call: $v0 = bin(n-1,k-1)
		add $s0, $zero, $v0, 0						# save bin(n-1,k-1) in s0
		add $a1, $a1, $imm, 1						# k=k+1 restoring for next call
		jal $imm, $zero, $zero, bin					#  recursive call: $v0 = bin(n-1,k)
		add $v0, $v0, $s0, 0						# v0 = bin(n-1,k-1) + bin(n-1,k)

		lw $ra, $sp, $imm, 0						# restore ra from 0($sp)
		lw $a0, $sp, $imm, 1						# restore a0 from 1($sp)
		lw $a1, $sp, $imm, 2						# restore a1 from 2($sp)
		lw $s0, $sp, $imm, 3						# restore s0 from 3($sp)
		add $sp, $sp, $imm, 4						# restore aloocated space from stack
		beq $ra, $zero, $zero, 0					# return to calling PC+1 
