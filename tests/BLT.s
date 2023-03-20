	.file	"loop.c"
	.option pic
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	li	a4,5
	li	a5,6
	blt	a4,a5,.L6
	li 	a6,10
	li	a7,10
.L6:	
	li	a4,6
	li	a5,5
	blt	a4,a5,.L5
	li	a6,10
	li	a7,10
.L5:	
	li	a4,-6
	li	a5,-5
	blt	a4,a5,.L4
	li	a6,10
	li	a7,10
.L4:	
	li	a4,-5
	li	a5,-5
	blt	a4,a5,.L3
	li	a6,10
	li	a7,10
.L3:
	li	a4,5
	li	a5,5
	blt	a4,a5,.L2
	li	a6,10
	li 	a7,10
	jr	ra
.L2:
	li	a5,10
	jr 	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 9.2.0"
	.section	.note.GNU-stack,"",@progbits
