	.text
Yadd:	pushq	%rbp
	movq	%rsp,%rbp
	addq	$-32, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	movb	%cl, -10(%rbp)
	movss	%xmm0, -18(%rbp)
	movq	-8(%rbp), %rdi
	movq	(%rdi), %rdi
	movq	$1, %rsi
	addq	%rsi, %rdi
	movq	-8(%rbp), %rsi
	movq	%rdi, (%rsi)
	xorq	%rdi, %rdi
	movb	$1, %dil
	cmpq	$0, %rdi
	setne	%dil
	movzx	%dil, %rdi
	movb	%dil, -9(%rbp)
	.data
L1:
	.byte	100
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.text
	movb	$100, -10(%rbp)
	.data
L2:
	.long	1097229926
	.text
	movss	L2(%rip), %xmm0
	movss	%xmm0, -18(%rbp)
	movss	-18(%rbp), %xmm0
	addq	$32, %rsp
	popq	%rbp
	ret
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp,%rbp
	addq	$-32, %rsp
	movq	$3, -8(%rbp)
	xorq	%rdi, %rdi
	movb	$0, %dil
	cmpq	$0, %rdi
	setne	%dil
	movzx	%dil, %rdi
	movb	%dil, -9(%rbp)
	.data
L3:
	.byte	99
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.text
	movb	$99, -10(%rbp)
	.data
L4:
	.long	1094923059
	.text
	movss	L4(%rip), %xmm0
	movss	%xmm0, -24(%rbp)
	leaq	-8(%rbp), %rdi
	movb	-9(%rbp), %sil
	movb	-10(%rbp), %cl
	movss	-24(%rbp), %xmm0
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
	addq	$32, %rsp
	popq	%rbp
	ret
