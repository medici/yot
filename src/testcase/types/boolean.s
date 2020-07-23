	.text
	.data
	.globl	Ytru
Ytru:	.byte	1
Ybb:	.byte	0
	.text
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-32, %rsp
	movq	$66, -8(%rbp)
	movq	$55, -16(%rbp)
	movq	$1, %rdi
	cmpq	$0, %rdi
	jne	L1                  
	movq	$1, %rdi
	cmpq	$0, %rdi
	je	L2                  
	movq	$0, %rdi
	cmpq	$0, %rdi
	xorq	$1, %rdi
	cmpq	$0, %rdi
	je	L2                  
L1:
	movq	$1, %rdi
	cmpq	$0, %rdi
	je	L3                  
	movq	$0, %rdi
	cmpq	$0, %rdi
	xorq	$1, %rdi
	cmpq	$0, %rdi
	je	L3                  
L2:
	movq	$0, %rdi
	cmpq	$0, %rdi
	xorq	$1, %rdi
	cmpq	$0, %rdi
	xorq	$1, %rdi
	cmpq	$0, %rdi
L3:
	movb	%dil, -17(%rbp)
	movb	-17(%rbp), %dil
	callq	YWriteBool
	callq	YWriteLn
	addq	$32, %rsp
	popq	%rbp
	retq
