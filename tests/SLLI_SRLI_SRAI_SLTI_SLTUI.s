	.file	"add1.c"
	.option pic
	.text
	.globl	__addsf3
	.align	2
	.globl	main
	.type	main, @function
main:
	li 	a5,1
	slli	a6,a5,1
	slli	a6,a5,31
	slli	a6,a5,34
	srli	a6,a5,2
	li	a5,-1
	srli	a6,a5,2
	srai	a6,a5,2
	li	a5,1024
	srai	a6,a5,2
	slti	a6,a5,2047
	slti	a6,a5,512
	slti	a6,a5,-1024
	slti	a6,a5,1024
	li	a5,-1024
	slti	a6,a5,-2048
	slti	a6,a5,-512
	slti	a6,a5,1024
	slti	a6,a5,-1024
	li	a5,1024
	sltiu	a6,a5,2047
	sltiu	a6,a5,512
	sltiu	a6,a5,-1024
	sltiu	a6,a5,1024
	li	a5,-1024
	sltiu	a6,a5,-2048
	sltiu	a6,a5,-512
	sltiu	a6,a5,1024
	sltiu	a6,a5,-1024
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
