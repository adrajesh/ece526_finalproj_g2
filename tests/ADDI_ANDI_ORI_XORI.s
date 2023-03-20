	.file	"add1.c"
	.option pic
	.text
	.globl	__addsf3
	.align	2
	.globl	main
	.type	main, @function
main:
	li 	a5,6
	addi	a6,a5,12
	addi	a6,a5,-12
	li 	a5,-6
	addi	a6,a5,12
	addi 	a6,a5,-12
	li	a5,6
	andi	a6,a5,12
	andi	a6,a5,-12
	li	a5,-6
	andi	a6,a5,12
	andi	a6,a5,-12
	li	a5,6
	ori	a6,a5,12
	ori	a6,a5,-12
	li	a5,-6
	ori	a6,a5,12
	ori	a6,a5,-12
	li	a5,6
	xori	a6,a5,12
	xori	a6,a5,-12
	li	a5,-6
	xori	a6,a5,12
	xori	a6,a5,-12
	jr	ra
	.size	main, .-main
	.section	.rodata
	.align	2
.LC0:
	.word	1084227584
	.align	2
.LC1:
	.word	1092616192
	.ident	"GCC: (GNU) 9.2.0"
	.section	.note.GNU-stack,"",@progbits
