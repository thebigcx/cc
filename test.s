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
	.comm	index, 4, 4
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
	movq	$0, %r8
	movl	%r8d, index(%rip)
L3:
	movslq	index(%rip), %r8
	movq	$10, %r9
	cmpq	%r9, %r8
	jge	L4
	jmp	L3
	movslq	index(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movl	%r9d, index(%rip)
	jmp	L3
L4:
L2:
	movq	%rbp, %rsp
	popq	%rbp
	ret

