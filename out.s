	.text
	.global main
	.comm x, 8, 8
	.comm y, 1, 1
main:
	pushq %rbp
	movq %rsp, %rbp
	movq $10, %r8
	movq %r8, x(%rip)
	movq $0, %r8
	movb %r8b, y(%rip)
L1:
	movzbq y(%rip), %r8
	movq $10, %r9
	cmpq %r9, %r8
	jge L2
	movzbq y(%rip), %r8
	movq $1, %r9
	subq %r9, %r8
	movb %r8b, y(%rip)
	movzbq y(%rip), %r8
	movq $1, %r9
	addq %r8, %r9
	movb %r9b, y(%rip)
	jmp L1
L2:
	movq %rbp, %rsp
	popq %rbp
	ret

