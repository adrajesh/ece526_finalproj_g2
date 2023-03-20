	.file	"loop.c"
	.option pic
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	li	a4,5
	li	a5,5
	beq	a4,a5,.L4
	li 	a6,10
	li	a7,10
.L4:	
	li	a4,-5
	li	a5,-5
	beq	a4,a5,.L3
	li	a6,10
	li	a7,10
.L3:
	li	a4,5
	li	a5,-5
	beq	a4,a5,.L2
	li	a6,10
	li 	a7,10
	jr	ra
.L2:
	li	a5,10
	jr 	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 9.2.0"
	.section	.note.GNU-stack,"",@progbits
