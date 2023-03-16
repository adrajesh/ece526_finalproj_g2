	.file	"add.c"
	.option pic
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	li a0,5
        beq a0,a1,5
	bne a0,a1,15
   	blt a0,a1,25
	bge a0,a1,35
	bltu a0,a1,45
	bgeu a0,a1,55
	jr ra
	.size	main, .-main
	.ident	"GCC: (GNU) 9.2.0"
	.section	.note.GNU-stack,"",@progbits
