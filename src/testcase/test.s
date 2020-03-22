	.text
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp,%rbp
	addq	$-816, %rsp
	movq	$10, -808(%rbp)
	movq	-808(%rbp), %rdi
	imulq	$8, %rdi
	addq	$-800, %rdi
	movq	$99, %rsi
	addq	%rbp, %rdi
	movq	%rsi, (%rdi)
	movq	-808(%rbp), %rdi
	imulq	$8, %rdi
	addq	$-800, %rdi
	addq	%rbp, %rdi
	movq	(%rdi), %rdi
	call	YWriteInt
	call	WriteLn
	addq	$816, %rsp
	popq	%rbp
	ret
