	.file	"add.c"
	.option pic
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	li a0,5
        addi a1,a0,5
	addi a2,a2,10
	addi a3,a2,2047
	slti a0,a1,2047
	sltiu a0,a1,-1
        xori a1,a0,2047
	xori a2,a2,10
        ori a1,a0,2047
	ori a2,a2,10
	jr ra
	.size	main, .-main
	.ident	"GCC: (GNU) 9.2.0"
	.section	.note.GNU-stack,"",@progbits
