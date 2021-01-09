		add $t0, $zero, $zero, 0		  	# i = 0
		add $t3, $zero, $imm, 1             # k = 1
		add $s0, $imm, $zero, 1023          # cycels = 1024
		add $s1, $imm, $zero, 31            # num_leds = 31

for:	
		add $t1, $zero, $zero, 0            # j = 0
		sll $t0, $t0, $imm, 1				# i << 1
		or $t0, $t0, $imm, 1                # i += 1
		sra $t2, $t0, $imm, 31              # n = 31 >> i
		and $t2, $t2, $imm, 0x1             # bitwise and n and 1
		beq $imm, $t2, $t3, exit            # if (n == 1) exit
		out $t0, $imm, $zero, 9		        # turn on one more led		
		beq $imm, $zero, $zero, wait        # go to wait 1024 cycles

wait:	
		add $t1, $t1, $imm, 1				# j += 1
		beq $imm, $t1, $s0, for		        # if (f == 1024) return to for loop
		beq $imm, $zero, $zero, wait

exit:	
		halt $zero, $zero, $zero, 0         # exit the program

