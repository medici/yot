	.text
	.data
	.globl	Ytru
Ytru:	.byte	1
	.text
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp,%rbp
	addq	$-32, %rsp
	movq	$66, -8(%rbp)
	movq	$55, -16(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	cmpq	%rsi, %rdi
	setg	%dil
	movzx	%dil, %rdi
	cmpq	$0, %rdi
	je		L1
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	cmpq	%rsi, %rdi
	setl	%dil
	movzx	%dil, %rdi
	cmpq	$0, %rdi
	je		L1
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	cmpq	%rsi, %rdi
	setg	%dil
	movzx	%dil, %rdi
	cmpq	$0, %rdi
	je		L2
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	cmpq	%rsi, %rdi
	setl	%dil
	movzx	%dil, %rdi
	cmpq	$0, %rdi
L2:
	jmp	L1
L1:
	setne	%dil
	movzx	%dil, %rdi
	movq	%rdi, -24(%rbp)
	movsb	-24(%rbp), %rdi
	call	WriteBool
	addq	$32, %rsp
	popq	%rbp
	ret
