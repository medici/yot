	.text
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp,%rbp
	addq	$-16, %rsp
	movq	$10, -8(%rbp)
	movq	$0, -16(%rbp)
L1:
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	cmpq	%rsi, %rdi
	setg	%dil
	movzx	%dil, %rdi
	cmpq	$0, %rdi
	je		L2
	movq	-16(%rbp), %rdi
	movq	$1, %rsi
	addq	%rsi, %rdi
	movq	%rdi, -16(%rbp)
	jmp	L1
L2:
	movq	-16(%rbp), %rdi
	movq	$20, %rsi
	cmpq	%rsi, %rdi
	setl	%dil
	movzx	%dil, %rdi
	cmpq	$0, %rdi
	je		L3
	movq	-16(%rbp), %rdi
	movq	$1, %rsi
	addq	%rsi, %rdi
	movq	%rdi, -16(%rbp)
	jmp	L1
L3:
	movq	-16(%rbp), %rdi
	call	YWriteInt
	call	YWriteLn
	addq	$16, %rsp
	popq	%rbp
	ret
