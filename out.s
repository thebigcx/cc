	.text
	.global main
main:
	pushq %rbp
	movq %rsp, %rbp
	.comm x, 8, 8
	movq $10, %r8
	movq %r8, x(%rip)
	movq x(%rip), %r8
	movq $40, %r9
	cmpq %r9, %r8
	jge L1
	movq $0, %r8
	movq %r8, x(%rip)
L1:
	movq %rbp, %rsp
	popq %rbp

