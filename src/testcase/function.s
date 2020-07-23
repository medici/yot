	.text
Yadd:	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rcx, -24(%rbp)
	movq	%rdx, -32(%rbp)
	movq	%r8, -48(%rbp)
	movq	%r9, -40(%rbp)
	movq	-8(%rbp), %rdi
	movq	(%rdi), %rdi
	movq	$1, %rsi
	addq	%rsi, %rdi
	movq	-8(%rbp), %rsi
	movq	%rdi, (%rsi)
	movq	$1, %rdi
	cmpq	$0, %rdi
	setne	%dil
	movzx	%dil, %rdi
	movq	-16(%rbp), %rsi
	movb	%dil, (%rsi)
	movq	$100, %rdi
	movq	-24(%rbp), %rsi
	movb	%dil, (%rsi)
	leaq	Y__Real__(%rip), %rdi
	movss	0(%rdi), %xmm0
	movq	-32(%rbp), %rdi
	movss	%xmm0, (%rdi)
	movq	$0, %rdi
	imul	$1, %rdi
	movq	-48(%rbp), %rsi
	addq	%rsi, %rdi
	movq	$110, %rsi
	movb	%sil, (%rdi)
	movq	-48(%rbp), %rdi
	movq	-40(%rbp), %rsi
	callq	YWriteString
	callq	YWriteLn
	movq	-8(%rbp), %rdi
	movq	(%rdi), %rdi
	addq	$48, %rsp
	popq	%rbp
	retq
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-48, %rsp
	leaq	-40(%rbp), %rdi
	leaq	Y__Str__(%rip), %rsi
	leaq	4(%rsi), %rsi
	movq	$4, %rcx
	cld
	rep	movsb
	movq	$3, -8(%rbp)
	movq	-8(%rbp), %rdi
	imul	$1, %rdi
	addq	$-40, %rdi
	addq	%rbp, %rdi
	movq	$97, %rsi
	movb	%sil, (%rdi)
	movq	$0, %rdi
	cmpq	$0, %rdi
	setne	%dil
	movzx	%dil, %rdi
	movb	%dil, -9(%rbp)
	movb	$99, -10(%rbp)
	leaq	Y__Real__(%rip), %rdi
	movss	4(%rdi), %xmm0
	movss	%xmm0, -24(%rbp)
	leaq	-8(%rbp), %rdi
	leaq	-9(%rbp), %rsi
	leaq	-10(%rbp), %rcx
	leaq	-24(%rbp), %rdx
	leaq	-40(%rbp), %r8
	movq	$12, %r9
	callq	Yadd
	movq	%rdi, -8(%rbp)
	addq	$48, %rsp
	popq	%rbp
	retq
	.data
Y__Real__:
	.long	1097229926
	.long	1094923059
Y__Str__:
	.byte	100
	.byte	0
	.byte	110
	.byte	0
	.byte	115
	.byte	116
	.byte	114
	.byte	0
	.byte	97
	.byte	0
	.byte	99
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
