	.file	"floatin.c"
	.option pic
	.text
	.section	.rodata
	.align	2
.LC2:
	.string	"The result of %.2f + %.2f is %.2f\n"
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	lla	a5,.LC0
	flw	fa5,0(a5)
	fsw	fa5,-20(s0)
	lla	a5,.LC1
	flw	fa5,0(a5)
	fsw	fa5,-24(s0)
	flw	fa4,-20(s0)
	flw	fa5,-24(s0)
	fadd.s	fa5,fa4,fa5
	fsw	fa5,-28(s0)
	flw	fa5,-20(s0)
	fcvt.d.s	fa4,fa5
	flw	fa5,-24(s0)
	fcvt.d.s	fa3,fa5
	flw	fa5,-28(s0)
	fcvt.d.s	fa5,fa5
	fsd	fa5,-40(s0)
	lw	a6,-40(s0)
	lw	a7,-36(s0)
	fsd	fa3,-40(s0)
	lw	a4,-40(s0)
	lw	a5,-36(s0)
	fsd	fa4,-40(s0)
	lw	a2,-40(s0)
	lw	a3,-36(s0)
	lla	a0,.LC2
	call	printf@plt
	li	a5,0
	mv	a0,a5
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	main, .-main
	.section	.rodata
	.align	2
.LC0:
	.word	1080033280
	.align	2
.LC1:
	.word	1074161254
	.ident	"GCC: (GNU) 9.2.0"
	.section	.note.GNU-stack,"",@progbits
