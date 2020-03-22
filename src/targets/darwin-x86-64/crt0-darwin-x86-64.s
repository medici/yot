#
	.text
	.globl	_main
_main:
	pushq	%rbp
	movq	%rsp,%rbp
	callq	Ymain
	popq %rbp
	retq


# https://gist.github.com/FiloSottile/7125822
	.text
	.globl Exit
Exit:
	movq   $0x2000001, %rax 
    movq   $0, %rdi
    syscall                       # _exit(0)

	.text
	.globl Write
Write:
	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-16, %rsp
	movq    $0x2000004, %rax  # write
	movq	$1, %rdx
	movq	$1, %rsi
	xchgq	%rdi,%rsi
	movq    %rsi, -16(%rbp)
	leaq    -16(%rbp), %rsi
	syscall
	addq	$16, %rsp
	popq 	%rbp
	retq