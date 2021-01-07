		lw $s2, $imm, $zero, 0x100     # r = &(0x100)
		add $t1, $zero, $imm, 21
		out $imm, $zero, $t1, 225      # set color to white 
first:		
		add $s0, $zero, $imm, 175      # x = 175
		add $s1, $zero, $imm, 143      # y = 143
		add $t0, $zero, $zero, 0       # layer = 0       
		sub $s0, $s0, $s2, 0           # x = x - r
		sub $t2, $s2, $t0, 0           # n = r - layer
		sll $t2, $t2, $imm, 1          # n = n*2
		add $t3, $zero, $zero, 0       # i = 0
		beq $imm, $zero, $zero, fill   # fill the pixels in the current layer
top:     
		add $t0, $t0, $imm, 1          # layer += 1
		beq $imm, $t0, $s2, second     # if (layer == r) go to second
		add $s0, $zero, $imm, 175      # x = 175
		add $s1, $zero, $imm, 143      # y = 143
		sub $s0, $s0, $s2, 0           # x = x - r
		add $s0, $s0, $t0, 0           # x = x + layer
		sub $t2, $s2, $t0, 0           # n = r - layer
		sll $t2, $t2, $imm, 1          # n = n*2
		add $s1, $s1, $t0, 0           # y = y + layer
		add $t3, $zero, $zero, 0       # i = 0
		beq $imm, $zero, $zero, fill   # fill the pixels in the current layer
		
fill1:
		add $t1, $zero, $imm, 19  
		out $s0, $zero, $t1, 0         # set x index
		add $t1, $zero, $imm, 20  
		out $s2, $zero, $t1, 0         # set y index
		add $t1, $zero, $imm, 18
		out $imm, $zero, $t1, 1        # monitorcmd = 1
		add $t3, $t3, $imm, 1          # i += 1
		beq $imm, $t3, $t2, top        # if (i == n) go to the next layer
		beq $imm, $zero, $zero, fill1  # else fill the next one
second:
		add $t0, $zero, $imm, 1        # layer = 1
		add $s0, $zero, $imm, 175      # x = 175
		add $s1, $zero, $imm, 143      # y = 143  
		sub $s0, $s0, $s2, 0           # x = x - r
		add $s0, $s0, $t0, 0           # x = x + layer
		sub $t2, $s2, $t0, 0           # n = r - layer
		sll $t2, $t2, $imm, 1          # n = n*2
		sub $s1, $s1, $t0, 0           # y = y - layer
		add $t3, $zero, $zero, 0       # i = 0
		beq $imm, $zero, $zero, fill2   # fill the pixels in the current layer
buttom:     
		add $t0, $t0, $imm, 1          # layer += 1
		beq $imm, $t0, $s2, exit       # if (layer == r) go to exit
		add $s0, $zero, $imm, 175      # x = 175
		add $s1, $zero, $imm, 143      # y = 143 
		sub $s0, $s0, $s2, 0           # x = x - r
		add $s0, $s0, $t0, 0           # x = x + layer
		sub $t2, $s2, $t0, 0           # n = r - layer
		sll $t2, $t2, $imm, 1          # n = n*2
		sub $s1, $s1, $t0, 0           # y = y - layer
		add $t3, $zero, $zero, 0       # i = 0
		beq $imm, $zero, $zero, fill2  # fill the pixels in the current layer

fill2:
		add $t1, $zero, $imm, 19  
		out $s0, $zero, $t1, 0         # set x index
		add $t1, $zero, $imm, 20  
		out $s2, $zero, $t1, 0         # set y index
		add $t1, $zero, $imm, 18
		out $imm, $zero, $t1, 1        # monitorcmd = 1
		add $t3, $t3, $imm, 1          # i += 1
		beq $imm, $t3, $t2, top        # if (i == n) go to the next layer
		beq $imm, $zero, $zero, fill2  # else fill the next one
exit:  
		halt $zero, $zero, $zero, 0



