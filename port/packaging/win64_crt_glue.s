/* Minimal x86_64 Windows CRT glue for linking Odin windows_amd64 COFF objects with lld-link + MinGW64 CRT. */
.data
.globl _fltused
_fltused:
	.long 0

.globl __CTOR_LIST__
__CTOR_LIST__:
	.quad 0
	.quad 0

.globl __DTOR_LIST__
__DTOR_LIST__:
	.quad 0
	.quad 0

.text
.globl __chkstk
.globl ___chkstk_ms
__chkstk:
___chkstk_ms:
	pushq %rcx
	pushq %rax
	cmpq $0x1000, %rax
	leaq 24(%rsp), %rcx
	jb 2f
1:
	subq $0x1000, %rcx
	orl $0, (%rcx)
	subq $0x1000, %rax
	cmpq $0x1000, %rax
	ja 1b
2:
	subq %rax, %rcx
	orl $0, (%rcx)
	popq %rax
	popq %rcx
	retq
