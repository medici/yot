	.text
	.data
Yrrr:	.long	-1078774989
Yr:	.long	0
	.text
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp,%rbp
	addq	$-32, %rsp
	leaq	Y__Real__(%rip), %rdi
	movss	4(%rdi), %xmm0
	movss	%xmm0, Yr(%rip)
	movq	Yr(%rip), %rdi
	movq	$2147483647, %rsi
	andq	%rsi, %rdi
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	call	YWriteInt
	call	YWriteLn
	addq	$32, %rsp
	popq	%rbp
	ret
	.data
Y__Real__:
	.long	1095132774
	.long	1147599258
