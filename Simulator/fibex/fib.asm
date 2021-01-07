	add $sp, $zero, $imm, 1		# 0: set $sp = 1
	sll $sp, $sp, $imm, 11		# 2: set $sp = 1 << 11 = 2048
	add $t0, $zero, $imm, 6		# 4: set $t0 = 6
	out $imm, $t0, $zero, L3	# 6: set irqhandler as L3
	add $t0, $zero, $imm, 1		# 8: set $t0 = 1
	out $t0, $zero, $imm, 2		# a: enable irq2
	add $t0, $zero, $imm, 176	# c: set $t0 = 176
	out $t0, $zero, $imm, 19	# e: write to monitorx
	add $t0, $zero, $imm, 72	# 10: set $t0 = 72
	out $t0, $zero, $imm, 20	# 12: write to monitory
	add $t0, $zero, $imm, 255	# 14: set $t0 = 255
	out $t0, $zero, $imm, 21	# 16: write to monitordata
	lw $a0, $zero, $imm, 1024	# 18: get x from address 1024
	jal $imm, $zero, $zero, fib	# 1a: calc $v0 = fib(x)
	sw $v0, $zero, $imm, 1025	# 1c: store fib(x) in 1025
	halt $zero, $zero, $zero, 0	# 1e: halt
fib:
	add $sp, $sp, $imm, -3		# 1f: adjust stack for 3 items
	sw $s0, $sp, $imm, 2		# 21: save $s0
	sw $ra, $sp, $imm, 1		# 23: save return address
	sw $a0, $sp, $imm, 0		# 25: save argument
	add $t0, $zero, $imm, 1		# 27: $t0 = 1
	bgt $imm, $a0, $t0, L1		# 29: jump to L1 if x > 1
	add $v0, $a0, $zero, 0		# 2b: otherwise, fib(x) = x, copy input
	beq $imm, $zero, $zero, L2	# 2c: jump to L2
L1:
	sub $a0, $a0, $imm, 1		# 2e: calculate x - 1
	jal $imm, $zero, $zero, fib	# 30: calc $v0=fib(x-1)
	add $s0, $v0, $imm, 0		# 32: $s0 = fib(x-1)
	lw $a0, $sp, $imm, 0		# 34: restore $a0 = x
	sub $a0, $a0, $imm, 2		# 36: calculate x - 2
	jal $imm, $zero, $zero, fib	# 38: calc fib(x-2)
	add $v0, $v0, $s0, 0		# 3a: $v0 = fib(x-2) + fib(x-1)
	lw $a0, $sp, $imm, 0		# 3b: restore $a0
	lw $ra, $sp, $imm, 1		# 3d: restore $ra
	lw $s0, $sp, $imm, 2		# 3f: restore $s0
L2:
	add $sp, $sp, $imm, 3		# 41: pop 3 items from stack
	beq $ra, $zero, $zero, 0	# 43: and return
L3:
	in $t3, $zero, $imm, 9		# 44: read leds register into $t3
	sll $t3, $t3, $imm, 1		# 46: left shift led pattern to the left
	or $t3, $t3, $imm, 1		# 48: lit up the rightmost led
	out $t3, $zero, $imm, 9		# 4a: write the new led pattern
	add $t3, $zero, $imm, 1		# 4c: set $t3 = 1
	out $t3, $zero, $imm, 18	# 4e: write to monitorcmd
	in $t3, $zero, $imm, 19		# 50: read monitorx register into $t3
	sub $t2, $imm, $t3, 367		# 52: $t2 = 367 - $t3
	out $t2, $zero, $imm, 19	# 54: set monitorx
	add $t2, $zero, $imm, 1		# 56: set $t2 = 1
	out $t2, $zero, $imm, 18	# 58: write to monitorcmd
	add $t3, $t3, $imm, 1		# 5a: $t3 = $t3 + 1
	out $t3, $zero, $imm, 19	# 5c: set monitorx
	in $t3, $zero, $imm, 20		# 5e: read monitory register into $t3
	add $t3, $t3, $imm, 1		# 60: $t3 = $t3 + 1
	out $t3, $zero, $imm, 20	# 62: set monitory
	out $zero, $zero, $imm, 5	# 64: clear irq2 status
	reti $zero, $zero, $zero, 0	# 66: return from interrupt
	.word 1024 7
