	.text
	.global main
pointer:
	pushq	%rbp
	movq	%rsp, %rbp
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
	.comm	s, 4, 4
	.comm	s2, 4, 4
	.comm	ptr, 8, 8
	.comm	array, 40, 40
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$8, %r10
	movl	%r10d, p(%rip)
	movl	%r10d, o(%rip)
	movq	$700, %r8
	movl	%r8d, s(%rip)
	movslq	s(%rip), %r8
	movl	%r8d, s2(%rip)
	leaq	o(%rip), %r8
	movq	%r8, ptr(%rip)
	movq	$100, %r8
	movq	ptr(%rip), %r9
	movq	%r8, (%r9)
	movq	$10, %r8
	movq	%r8, %rdi
	call	pointer
	movq	%rax, %r9
	movq	%r9, ptr(%rip)
	movq	$10, %r8
	leaq	array(%rip), %r9
	movq	$3, %r10
	movq	$4, %r11
	imulq	%r10, %r11
	addq	%r9, %r11
L2:
	movq	%rbp, %rsp
	popq	%rbp
	ret

