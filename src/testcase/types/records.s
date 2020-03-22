	.text
	.data
Ys:	.quad	0
	.quad	0
	.quad	0
	.quad	0
	.quad	0
	.quad	0
	.text
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp,%rbp
	leaq	Ys(%rip), %rdi
	addq	$0, %rdi
	movq	$10, %rsi
	movq	%rsi, (%rdi)
	leaq	Ys(%rip), %rdi
	addq	$0, %rdi
	movq	(%rdi), %rdi
	call	YWriteInt
	call	YWriteLn
	leaq	Ys(%rip), %rdi
	addq	$8, %rdi
	.data
L1:
	.byte	115
	.byte	116
	.byte	114
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.text
	leaq	L1(%rip), %rsi
	xorq	%rcx, %rcx
L2:
	movb	(%rsi), %cl
	movb	%cl, (%rdi)
	addq	$1, %rsi
	addq	$1, %rdi
	cmpq	$0, %rcx
	jne		L2
	leaq	Ys(%rip), %rdi
	addq	$8, %rdi
	movq	$34, %rsi
	call	YWriteString
	call	YWriteLn
	popq	%rbp
	ret
