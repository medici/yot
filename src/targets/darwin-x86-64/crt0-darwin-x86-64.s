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

	.text
	.globl Brk
Brk:
	pushq	%rbp
	movq	%rsp, %rbp
	movq  $0x20000c5, %rax  # mmap
	movq  $0, %rsi
	xchgq	%rdi,%rsi
	movq  $3,  %rdx # PROT_READ | PROT_WRITE
	movq  $4098, %r10 # MAP_PRIVATE | MAP_ANON
	movq  $-1,  %r8
	movq  $0, %r9
	syscall
	movq  %rax, %rdi
	popq 	%rbp
	retq


	.text
	.globl Free
Free:
	pushq	%rbp
	movq	%rsp, %rbp
	movq  $0x2000049, %rax  # munmap
	syscall
	movq  %rax, %rdi
	popq 	%rbp
	retq

  .text
	.globl Memset
Memset:
	pushq	%rbp
	movq	%rsp, %rbp
  rep     stosq 
	popq 	%rbp
	retq