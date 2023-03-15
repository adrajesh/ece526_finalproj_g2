	.file	"add.c"
	.option pic
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	li a0,5
        jalr a1,a0,7
        jal a2,16
	jr ra
	.size	main, .-main
	.ident	"GCC: (GNU) 9.2.0"
	.section	.note.GNU-stack,"",@progbits
