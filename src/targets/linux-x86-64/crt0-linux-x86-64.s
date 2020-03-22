#
	.text
	.globl	_start
_start:
	callq	Ymain
	callq  Exit


# https://gist.github.com/FiloSottile/7125822
	.text
	.globl Exit
Exit:
	movq   $60, %rax 
    movq   $0, %rdi
    syscall                       # _exit(0)

	.text
	.globl Write
Write:
	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-16, %rsp
	movq    $1, %rax  # write
	movq	$1, %rdx
	movq	$1, %rsi
	xchgq	%rdi,%rsi
	movq    %rsi, -16(%rbp)
	leaq    -16(%rbp), %rsi
	syscall
	addq	$16, %rsp
	popq 	%rbp
	retq

