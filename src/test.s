	.text
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp,%rbp
	addq	$-528, %rsp
	movq	$0, -520(%rbp)
	leaq	-256(%rbp), %rdi
	leaq	Y__Str__(%rip), %rsi
	leaq	5(%rsi), %rsi
	movq  $3, %rcx
	cld
   rep	movsb
	leaq	-256(%rbp), %rdi
	movq	$256, %rsi
	call	YWriteString
	call	YWriteLn
	leaq	Y__Str__(%rip), %rdi
	leaq	0(%rdi), %rdi
	movq	$0, %rsi
	call	YWriteString
	call	YWriteLn
	addq	$528, %rsp
	popq	%rbp
	ret
	.data
Y__Str__:
	.byte	97
	.byte	98
	.byte	99
	.byte	57
	.byte	0
	.byte	115
	.byte	116
	.byte	114
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0

