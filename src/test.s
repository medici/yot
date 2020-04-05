	.text
Yadd:	pushq	%rbp
	movq	%rsp,%rbp
	addq	$-32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rcx, -17(%rbp)
	movss	%xmm0, -25(%rbp)
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
	movq	-16(%rbp), %rsi
	movb	%dil, (%rsi)
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
	movb	$100, -17(%rbp)
	.data
L2:
	.long	1097229926
	.text
	movss	L2(%rip), %xmm0
	movss	%xmm0, -25(%rbp)
	movss	-25(%rbp), %xmm0
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
	leaq	-9(%rbp), %rsi
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