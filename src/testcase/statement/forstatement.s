	.text
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-32, %rsp
	movq	$10, -8(%rbp)
	movq	$0, -16(%rbp)
	movq	$1, %rdi
L1:
	movq	-8(%rbp), %rsi
	cmpq	%rsi, %rdi
	jg	L2                  
	movq	%rdi, -24(%rbp)
	movq	-16(%rbp), %rdi
	movq	$1, %rsi
	addq	%rsi, %rdi
	movq	%rdi, -16(%rbp)
	movq	-24(%rbp), %rdi
	addq	$2, %rdi
	jmp	L1                  
L2:
	movq	-16(%rbp), %rdi
	callq	YWriteInt
	callq	YWriteLn
	movq	-8(%rbp), %rdi
	callq	YWriteInt
	callq	YWriteLn
	movq	-24(%rbp), %rdi
	callq	YWriteInt
	callq	YWriteLn
	addq	$32, %rsp
	popq	%rbp
	retq
