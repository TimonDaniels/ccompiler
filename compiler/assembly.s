	.text
.LC0:
	.string	"%d\n"
printint:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	ret

	.comm	i,8,8
	.comm	j,8,8
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$20, %r8
	movq	%r8, j(%rip)
	movq	j(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$10, %r8
	movq	%r8, i(%rip)
	movq	i(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$1, %r8
	movq	%r8, i(%rip)
L1:
	movq	i(%rip), %r8
	movq	$5, %r9
	cmpq	%r9, %r8
	jg	L2
	movq	i(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	i(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, i(%rip)
	jmp	L1
L2:
	movq	$253, %r8
	movq	%r8, j(%rip)
L3:
	movq	j(%rip), %r8
	movq	$256, %r9
	cmpq	%r9, %r8
	je	L4
	movq	j(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	j(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, j(%rip)
	jmp	L3
L4:
	movl	$0, %eax
	popq	%rbp
	ret
