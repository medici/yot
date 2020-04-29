

	.text
	.data
Yrrr:	.long	1078774989
Yr:	.long	0
	.text
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp,%rbp
	addq	$-16, %rsp
	leaq	YReal(%rip), %rdi
	movss	4(%rdi), %xmm0
	movss	%xmm0, -16(%rbp)

	movss $0, %xmm0
	flds	-16(%rbp)
	fsub   %xmm0
	fsts	-16(%rbp)
	movss	-16(%rbp), %xmm0

	call	YWriteReal
	call	YWriteLn
	addq	$16, %rsp
	popq	%rbp
	ret
	.data
YReal:
	.long	1095132774
	.long	1049310003
