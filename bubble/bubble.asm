		sub $sp, $sp, $imm, 3		  		# adjust stack for 3 items
		sw $s0, $sp, $imm, 0				# save s0 in 0($sp)
		sw $s1, $sp, $imm, 1				# save s1 in 1($sp)
		sw $s2, $sp, $imm, 2				# save s2 in 2($sp)

		add $s0, $zero, $imm, 1024		    # s0 = 1024
		add $s1, $zero, $zero, 0			# i = 0	
for1:	
		sub $t1, $s1, $imm, 15				# t1 = i - 15
		bgt $imm, $t1, $zero, exit1		    # if i > 15 go to exitloop1
		add $s2, $zero, $zero, 0			# j = 0
for2:	
		sub $t2, $s2, $imm, 14				# t2 = j - 14
		bgt $imm, $t2, $zero, exit2		    # if j > 14 go to exitloop2
		lw $t0, $s0, $s2, 0                 # load arr[j]
		add $t2, $s2, $imm, 1				# j + 1
		lw $t1, $t2, $s0, 0				    # load arr[j + 1]
		bge $imm, $t1, $t0, else		    # if arr[j + 1] > arr[j] 
		sw $t1, $s0, $s2, 0			     	# arr[j] = arr[j + 1]
		sw $t0, $t2, $s0, 0			     	# arr[j + 1] = arr[j] 
else:	
		add $s2, $s2, $imm, 1				# j += 1
		beq $imm, $zero, $zero, for2		# go to for2 
exit2: 		
		add $s1, $s1, $imm, 1				# i += 1
		jal $imm, $zero, $zero, for1		# go to for1 
exit1:
		lw $s0, $sp, $zero, 0				# restore s0 from sp
		lw $s1, $sp, $zero, 1				# restore s1 from sp	
		lw $s2, $sp, $zero, 2				# restore s2 from sp	
		add $sp, $sp, $imm, 3				# pop 3 items from stack			
		halt $zero, $zero, $zero, 0			# finish and exit program