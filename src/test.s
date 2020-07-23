	.text
	.data
Yaddr:	.quad	0
	.text
	.globl	YtestAdr
YtestAdr:	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-32, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	leaq	-9(%rbp), %rdi
	movq	%rdi, -25(%rbp)
	movq	-25(%rbp), %rdi
	callq	YWriteInt
	callq	YWriteLn
	leaq	-8(%rbp), %rdi
	movq	%rdi, -25(%rbp)
	movq	-25(%rbp), %rdi
	callq	YWriteInt
	callq	YWriteLn
	leaq	-17(%rbp), %rdi
	movq	%rdi, -25(%rbp)
	movq	-25(%rbp), %rdi
	callq	YWriteInt
	callq	YWriteLn
	leaq	-25(%rbp), %rdi
	movq	%rdi, -25(%rbp)
	movq	-25(%rbp), %rdi
	callq	YWriteInt
	callq	YWriteLn
	addq	$32, %rsp
	popq	%rbp
	retq
	.globl	YtestGet
YtestGet:	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-32, %rsp
	movq	$2, -8(%rbp)
	leaq	-8(%rbp), %rdi
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rdi
	movq	(%rdi), %rdi
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rdi
	callq	YWriteInt
	callq	YWriteLn
	addq	$32, %rsp
	popq	%rbp
	retq
	.globl	YtestPut
YtestPut:	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-32, %rsp
	movq	$2, -8(%rbp)
	leaq	-16(%rbp), %rdi
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rdi
	movq	-8(%rbp), %rsi
	movq	%rsi, (%rdi)
	movq	-16(%rbp), %rdi
	callq	YWriteInt
	callq	YWriteLn
	addq	$32, %rsp
	popq	%rbp
	retq
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-32, %rsp
	movq	$16384, -24(%rbp)
	movq	$3, %rdi
	movq	$8, %rsi
	movq	%rdi, %rax
	imul	%rsi
	movq	%rax, %rdi
	movq	-24(%rbp), %rsi
	subq	%rsi, %rdi
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rdi
	callq	YWriteInt
	callq	YWriteLn
	addq	$32, %rsp
	popq	%rbp
	retq
