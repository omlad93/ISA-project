		sub $sp, $sp, $imm, 3		  		# adjust stack for 3 items	(every enter is 4 byte size so only need to sub 3 places for 3 items)
		sw $s0, $sp, $imm, 0				# save s0 in 0($sp)
		sw $s1, $sp, $imm, 1				# save s1 in 1($sp)
		sw $s2, $sp, $imm, 2				# save s2 in 2($sp)

		add $s0, $zero, $imm, 0x1024		# s0 = 0x1024
		add $s1, $zero, $zero, 0			# s1 = 0	(i=0)	


for1:	
		sub $t1, $s1, $imm, 15				# t1 = i - 15
		bgt $imm, $t1, $zero, exitloop1		#if (i-15)>0 go to exitloop1
		add $s2, $zero, $zero, 0			# s2 = 0	(j=0)

for2:	
		sub $t2, $s2, $imm, 14				# t2 = j - 14
		bgt $imm, $t2, $zero, exitloop2		# if (j-14)>0 go to exitloop2

		add $t0, $s0, $s2, 0				# t0 = 0x1024 + j
		lw  $t1, $t0, $zero, 0				# t1 = Array[0x1024 + j]
		add $t2, $s2, $imm, 1				# t2 = j+1
		add $t2, $s0, $t2, 0				# t2 = 0x1024 + j+1
		lw  $t3, $t2, $zero, 0				# t3 = Array[0x1024 + j + 1]
		bgt $imm, $t1, $t3, 0				# if(t1 > t3) - go to else
		sw $t3, $t0, $zero, 0				# save t3 to Array[0x1024 + j]
		sw $t1, $t2, $zero, 0				# save t1 to Array[0x1024 + j + 1] - switching 

else:	
		add $s2, $s2, $imm, 1				# j ++	(s2 = j + 1)
		jal $imm, $zero, $zero, for2		# jump to for2 

exitloop2: 
		
		add $s1, $s1, $imm, 1				# i ++ (s1 = i + 1)
		jal $imm, $zero, $zero, for1		# jump to for1 

exitloop1:

		lw $s0, $sp, $zero, 0						# restore s0 from sp
		lw $s1, $sp, $zero, 1						# restore s1 from sp	
		lw $s2, $sp, $zero, 2						# restore s2 from sp	
		add $sp, $sp, $imm, 3						# pop 3 items from stack			
		halt $zero, $zero, $zero, 0					# finish and exit program