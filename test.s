	.section .text
	.global	pointer
pointer:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$0, %rsp
	movq	$0, %r8
	movq	%r8, %rax
	jmp	L1
	leaq	pointer(%rip), %r9
L1:
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.comm	o, 4, 4
	.comm	p, 4, 4
	.section .rodata
L3:
	.string	"Hello, world!"
	.section .text
	.global	main
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$80, %rsp
	movq	$8, %r10
	movl	%r10d, p(%rip)
	movl	%r10d, o(%rip)
	movq	$700, %r8
	movl	%r8d, -4(%rbp)
	movslq	-4(%rbp), %r8
	movl	%r8d, -8(%rbp)
	leaq	o(%rip), %r8
	movq	%r8, -16(%rbp)
	movq	$100, %r8
	movq	-16(%rbp), %r9
	movq	%r8, (%r9)
	movq	$10, %r8
	movq	%r8, %rdi
	call	pointer
	movq	%rax, %r9
	movq	%r9, -16(%rbp)
	leaq	L3(%rip), %r8
	movq	%r8, -24(%rbp)
	movq	$39, %r8
	movl	%r8d, -28(%rbp)
	leaq	-24(%rbp), %r8
	movq	$0, %r9
	movq	$1, %r10
	imulq	%r9, %r10
	addq	%r8, %r10
	movzbq	(%r10), %r10
	movl	%r10d, -28(%rbp)
	movq	$10, %r8
	leaq	-68(%rbp), %r9
	movq	$3, %r10
	movq	$4, %r11
	imulq	%r10, %r11
	addq	%r9, %r11
L2:
	movq	%rbp, %rsp
	popq	%rbp
	ret

