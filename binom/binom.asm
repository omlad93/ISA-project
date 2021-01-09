		add $sp, $zero, $imm, 4095
		lw $a0, $zero, $imm, 0x100                  # load the value of n from the memory
		lw $a1, $zero, $imm, 0x101                  # load the value of k from the memory
		bgt $imm, $a1, $a0, stop                    # if k > n goto stop
		beq $imm, $a1, $a0, k_eq_n                  # if k = n goto k_eq_n
		jal $imm, $zero, $zero, bin                 # go to binom function
		sw $v0, $zero, $imm, 0x102                  # store the value that was returned from the function
		beq $imm, $zero, $zero, stop                # goto stop
k_eq_n:
		add $t0, $zero, $imm, 1              
		sw $t0, $zero, $imm, 0x102                  # srote bin(n, k) = 1 
stop:
		halt $zero, $zero, $zero, 0

bin:	
		sub	$sp, $sp, $imm, 4						# adjust stack for 4 items	(every enter is 4 byte size so only need to sub 4 places for 4 items)
		sw $ra, $sp, $imm, 0						# save ra in 0($sp)
		sw $a0, $sp, $imm, 1						# save a0 in 1($sp)
		sw $a1, $sp, $imm, 2						# save a1 in 2($sp)
		sw $s0, $sp, $imm, 3						# save s0 in 3($sp)
 
		add $v0, $zero, $imm, 1						# set v0 = 1
		beq $imm, $a1, $zero, return				# if k == 0 goto return
		beq $imm, $a1, $a0, return					# if k == n goto return 

		sub $a0, $a0, $imm, 1						# n -= 1
		sub $a1, $a1, $imm, 1						# k -= 1 
		jal $imm, $zero, $zero, bin					# recursive call: $v0 = bin(n - 1, k - 1)
		add $s0, $zero, $v0, 0						# s0 = bin(n - 1, k - 1)
		add $a1, $a1, $imm, 1						# k += 1 
		jal $imm, $zero, $zero, bin					# recursive call: $v0 = bin(n - 1, k)
		add $v0, $v0, $s0, 0						# v0 = bin(n - 1, k - 1) + bin(n - 1, k)

return:		
		lw $ra, $sp, $imm, 0						# restore ra from 0($sp)
		lw $a0, $sp, $imm, 1						# restore a0 from 1($sp)
		lw $a1, $sp, $imm, 2						# restore a1 from 2($sp)
		lw $s0, $sp, $imm, 3						# restore s0 from 3($sp)
		add $sp, $sp, $imm, 4						# restore aloocated space from stack
		beq $ra, $zero, $zero, 0					# return

