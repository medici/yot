	.text
	.data
	.globl	Ywordsize2
Ywordsize2:	.long	1143383654
Ywordsize3:	.long	1171270861
Yii:	.long	0
Yjj:	.long	0
L1:
	.long	1171272909
	.text
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp,%rbp
	addq	$-32, %rsp
	.data
L2:
	.long	1232348160
	.text
	movss	L2(%rip), %xmm0
	movss	%xmm0, -8(%rbp)
	movss	-8(%rbp), %xmm0
	call	YWriteReal
	call	YWriteLn
	.data
L3:
	.long	953267991
	.text
	movss	L3(%rip), %xmm0
	movss	%xmm0, -8(%rbp)
	movss	-8(%rbp), %xmm0
	call	YWriteReal
	call	YWriteLn
	movss	Ywordsize2(%rip), %xmm0
	call	YWriteReal
	call	YWriteLn
	movss	L1(%rip), %xmm0
	call	YWriteReal
	call	YWriteLn
	.data
L4:
	.long	1073741824
	.text
	movss	L4(%rip), %xmm0
	movss	%xmm0, -8(%rbp)
	movss	-8(%rbp), %xmm0
	call	YWriteReal
	call	YWriteLn
	.data
L5:
	.long	1101004800
	.text
	movss	L5(%rip), %xmm0
	movss	%xmm0, -16(%rbp)
	movss	-16(%rbp), %xmm0
	call	YWriteReal
	call	YWriteLn
	.data
L6:
	.long	1092616192
	.text
	movss	L6(%rip), %xmm0
	movss	%xmm0, -24(%rbp)
	movss	-24(%rbp), %xmm0
	call	YWriteReal
	call	YWriteLn
	movss	-16(%rbp), %xmm0
	movss	-24(%rbp), %xmm1
	mulss	%xmm1, %xmm0
	movss	%xmm0, -16(%rbp)
	movss	-16(%rbp), %xmm0
	call	YWriteReal
	call	YWriteLn
	movss	-16(%rbp), %xmm0
	movss	-24(%rbp), %xmm1
	divss	%xmm1, %xmm0
	movss	%xmm0, -16(%rbp)
	movss	-16(%rbp), %xmm0
	call	YWriteReal
	call	YWriteLn
	movss	-16(%rbp), %xmm0
	cvtss2si	%xmm0, %rdi
	movss	-24(%rbp), %xmm0
	cvtss2si	%xmm0, %rsi
	movq	%rdi, %rax
	cqo
	idivq	%rsi
	movq	%rax, %rdi
	cvtsi2ss	%rdi, %xmm0
	movss	%xmm0, -16(%rbp)
	movss	-16(%rbp), %xmm0
	call	YWriteReal
	call	YWriteLn
	.data
L7:
	.long	1095761920
	.text
	cvtss2si	L7(%rip), %rdi
	.data
L8:
	.long	1094713344
	.text
	cvtss2si	L8(%rip), %rsi
	movq	%rdi, %rax
	cqo
	idivq	%rsi
	movq	%rdx, %rdi
	cvtsi2ss	%rdi, %xmm0
	movss	%xmm0, -16(%rbp)
	movss	-16(%rbp), %xmm0
	call	YWriteReal
	call	YWriteLn
	addq	$32, %rsp
	popq	%rbp
	ret
