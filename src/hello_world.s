	.text
	.data
L1:
	.byte	104
	.byte	101
	.byte	108
	.byte	108
	.byte	111
	.byte	32
	.byte	119
	.byte	111
	.byte	114
	.byte	108
	.byte	100
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.text
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp,%rbp
	leaq	L1(%rip), %rdi
	movq	$0, %rsi
	call	YWriteString
	call	YWriteLn
	popq	%rbp
	ret
