	.text
	.global main
	.comm o, 4, 4
hello:
	pushq %rbp
	movq %rsp, %rbp
	movq $10, %r8
	movl %r8d, o(%rip)
L1:
	movq %rbp, %rsp
	popq %rbp
	ret

printf:
	pushq %rbp
	movq %rsp, %rbp
	movq $0, %r9
	jmp L1
L2:
	movq %rbp, %rsp
	popq %rbp
	ret

	.comm x, 4, 4
	.comm y, 1, 1
main:
	pushq %rbp
	movq %rsp, %rbp
	movq $10, %r10
	movl %r10d, x(%rip)
	movq $0, %r8
	movb %r8b, y(%rip)
L4:
	movzbq y(%rip), %r8
	movq $10, %r9
	cmpq %r9, %r8
	jge L5
	movzbq y(%rip), %r8
	movq $1, %r9
	subq %r9, %r8
	movb %r8b, y(%rip)
	movq $8, %r8
	movq %r8, %rdi
	call printf
	movq %rax, %r9
	movzbq y(%rip), %r8
	movq $1, %r9
	addq %r8, %r9
	movb %r9b, y(%rip)
	jmp L4
L5:
L3:
	movq %rbp, %rsp
	popq %rbp
	ret

