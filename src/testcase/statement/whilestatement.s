	.text
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-16, %rsp
	movq	$10, -8(%rbp)
	movq	$0, -16(%rbp)
L1:
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	cmpq	%rsi, %rdi
	setg	%dil
	movzx	%dil, %rdi
	jle	L2                  
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
	jge	L3                  
	movq	-16(%rbp), %rdi
	movq	$1, %rsi
	addq	%rsi, %rdi
	movq	%rdi, -16(%rbp)
	jmp	L1                  
L3:
	movq	-8(%rbp), %rdi
	movq	$1, %rsi
	cmpq	%rsi, %rdi
	setg	%dil
	movzx	%dil, %rdi
	jle	L4                  
	movq	-8(%rbp), %rdi
	movq	$1, %rsi
	subq	%rsi, %rdi
	movq	%rdi, -8(%rbp)
	jmp	L1                  
L4:
	movq	-16(%rbp), %rdi
	callq	YWriteInt
	callq	YWriteLn
	movq	-8(%rbp), %rdi
	callq	YWriteInt
	callq	YWriteLn
	addq	$16, %rsp
	popq	%rbp
	retq
