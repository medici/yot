	.text
Yadd:	pushq	%rbp
	movq	%rsp,%rbp
	addq	$-48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rcx, -24(%rbp)
	movq	%rdx, -32(%rbp)
	movq	%r9, -40(%rbp)
	movq	%r8, -48(%rbp)
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
	leaq	YReal(%rip), %rdi
	movss	0(%rdi), %xmm0
	movq	-32(%rbp), %rdi
	movss	%xmm0, (%rdi)
	movq	$0, %rdi
	imul	$1, %rdi
	movq	-48(%rbp), %rsi
	addq	%rsi, %rdi
	movq	$50, %rsi
	movb	%sil, (%rdi)
	movq	-32(%rbp), %rdi
	movss	(%rdi), %xmm0
	addq	$48, %rsp
	popq	%rbp
	ret
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp,%rbp
	addq	$-48, %rsp
	leaq	-40(%rbp), %rdi
	leaq	YStr(%rip), %rsi
	leaq	4(%rsi), %rsi
L1:
	movsb	(%rsi), %rcx
	movb	%cl, (%rdi)
	addq	$1, %rsi
	addq	$1, %rdi
	cmpq	$0, %rcx
	jne		L1
	movq	$3, -8(%rbp)
	movq	$0, %rdi
	cmpq	$0, %rdi
	setne	%dil
	movzx	%dil, %rdi
	movb	%dil, -9(%rbp)
	movb	$99, -10(%rbp)
	leaq	YReal(%rip), %rdi
	movss	4(%rdi), %xmm0
	movss	%xmm0, -24(%rbp)
	leaq	-8(%rbp), %rdi
	leaq	-9(%rbp), %rsi
	leaq	-10(%rbp), %rcx
	leaq	-24(%rbp), %rdx
	leaq	-40(%rbp), %r8
	movq	$12, %r9
	call	Yadd
	movss	%xmm0, -24(%rbp)
	movq	-8(%rbp), %rdi
	call	YWriteInt
	call	YWriteLn
	movb	-9(%rbp), %dil
	call	YWriteBool
	call	YWriteLn
	movb	-10(%rbp), %dil
	call	Write
	call	YWriteLn
	movss	-24(%rbp), %xmm0
	call	YWriteReal
	call	YWriteLn
	leaq	-40(%rbp), %rdi
	movq	$12, %rsi
	call	YWriteString
	call	YWriteLn
	addq	$48, %rsp
	popq	%rbp
	ret
	.data
YReal:
	.long	1097229926
	.long	1094923059
YStr:
	.byte	100
	.byte	0
	.byte	50
	.byte	0
	.byte	115
	.byte	116
	.byte	114
	.byte	0
	.byte	99
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
