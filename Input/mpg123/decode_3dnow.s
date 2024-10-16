/
/ decode_3dnow.s - 3DNow! optimized synth_1to1()
/
/ This code based 'decode_3dnow.s' by Syuuhei Kashiyama
/ <squash@mb.kcom.ne.jp>,only two types of changes have been made:
/
/ - remove PREFETCH instruction for speedup
/ - change function name for support 3DNow! automatic detect
/ - femms moved to before 'call dct64_3dnow'
/
/ You can find Kashiyama's original 3dnow! support patch
/ (for mpg123-0.59o) at
/ http://user.ecc.u-tokyo.ac.jp/~g810370/linux-simd/ (Japanese).
/
/ by KIMURA Takuhiro <kim@hannah.ipc.miyakyo-u.ac.jp> - until 31.Mar.1999
/                    <kim@comtec.co.jp>               - after  1.Apr.1999
/
/ porting xmms-0.9.1 by Osamu kayasono <kayasono@mb.infoweb.ne.jp>
/ - rename function name for xmms
/ - disable equalizer for mpg123

///
/// Replacement of synth_1to1() with AMD's 3DNow! SIMD operations support
/// 
/// Syuuhei Kashiyama <squash@mb.kcom.ne.jp>
/// 
/// The author of this program disclaim whole expressed or implied
/// warranties with regard to this program, and in no event shall the
/// author of this program liable to whatever resulted from the use of
/// this program. Use it at your own risk.
/// 

	.local	buffs.40
	.comm	buffs.40,4352,32
.data
	.align 4
	.type	 bo.42,@object
	.size	 bo.42,4
bo.42:
	.long 1
.text
.globl mpg123_synth_1to1_3dnow
	.type	 mpg123_synth_1to1_3dnow,@function
mpg123_synth_1to1_3dnow:
	subl $24,%esp
	pushl %ebp
	pushl %edi
	xorl %ebp,%ebp
	pushl %esi
	pushl %ebx
	movl 56(%esp),%esi
	movl 52(%esp),%edi
	movl 0(%esi),%esi
	movl 48(%esp),%ebx
	addl %edi,%esi
	movl %esi,16(%esp)

	femms
		
///	/ disable equalizer by Osamu Kayasono
///	cmpl $0,equalfile
///	je .L25
///	pushl %ebx
///	pushl 48(%esp)
///	call do_equalizer_3dnow
///	addl $8,%esp
///.L25:
	testl %ebx,%ebx
	jne .L26
	decl bo.42
	movl $buffs.40,%ecx
	andl $15,bo.42
	jmp .L27
.L26:
	addl $2,16(%esp)
	movl $buffs.40+2176,%ecx
.L27:
	movl bo.42,%edx
	testb $1,%dl
	je .L28
	movl %edx,36(%esp)
	movl %ecx,%ebx
	movl 44(%esp),%esi
	movl %edx,%edi
	pushl %esi
	sall $2,%edi
	movl %ebx,%eax
	movl %edi,24(%esp)
	addl %edi,%eax
	pushl %eax
	movl %edx,%eax
	incl %eax
	andl $15,%eax
	leal 1088(,%eax,4),%eax
	addl %ebx,%eax
	pushl %eax
	call mpg123_dct64_3dnow
	addl $12,%esp
	jmp .L29
.L28:
	leal 1(%edx),%esi
	movl 44(%esp),%edi
	movl %esi,36(%esp)
	leal 1092(%ecx,%edx,4),%eax
	pushl %edi
	leal 1088(%ecx),%ebx
	pushl %eax
	sall $2,%esi
	leal (%ecx,%edx,4),%eax
	pushl %eax
	call mpg123_dct64_3dnow
	addl $12,%esp
	movl %esi,20(%esp)
.L29:
	movl $mpg123_decwin+64,%edx
	movl $16,%ecx
	subl 20(%esp),%edx
	movl 16(%esp),%edi

	movq (%edx),%mm0
	movq (%ebx),%mm1
	.align 32
.L33:
	movq 8(%edx),%mm3
        pfmul %mm1,%mm0
	movq 8(%ebx),%mm4
	movq 16(%edx),%mm5
  	pfmul %mm4,%mm3
	movq 16(%ebx),%mm6
  	pfadd %mm3,%mm0
	movq 24(%edx),%mm1
	pfmul %mm6,%mm5
	movq 24(%ebx),%mm2
	pfadd %mm5,%mm0
	movq 32(%edx),%mm3
	pfmul %mm2,%mm1
	movq 32(%ebx),%mm4
	pfadd %mm1,%mm0
	movq 40(%edx),%mm5
	pfmul %mm4,%mm3
	movq 40(%ebx),%mm6
	pfadd %mm3,%mm0
	movq 48(%edx),%mm1
	pfmul %mm6,%mm5
	movq 48(%ebx),%mm2
	pfadd %mm0,%mm5
	movq 56(%edx),%mm3
	pfmul %mm1,%mm2
	movq 56(%ebx),%mm4
	pfadd %mm5,%mm2
	addl $64,%ebx
	subl $-128,%edx
	movq (%edx),%mm0
	pfmul %mm4,%mm3
	movq (%ebx),%mm1
	pfadd %mm3,%mm2
	movq %mm2,%mm3
	psrlq $32,%mm3
	pfsub %mm3,%mm2
	incl %ebp
	pf2id %mm2,%mm2
	packssdw %mm2,%mm2
	movd %mm2,%eax
	movw %ax,0(%edi)
	addl $4,%edi
	decl %ecx
	jnz .L33
	
	movd (%ebx),%mm0
	movd (%edx),%mm1
	punpckldq 8(%ebx),%mm0
	punpckldq 8(%edx),%mm1
	movd 16(%ebx),%mm3
	movd 16(%edx),%mm4
	pfmul %mm1,%mm0
	punpckldq 24(%ebx),%mm3
	punpckldq 24(%edx),%mm4
	movd 32(%ebx),%mm5
	movd 32(%edx),%mm6
	pfmul %mm4,%mm3
	punpckldq 40(%ebx),%mm5
	punpckldq 40(%edx),%mm6
	pfadd %mm3,%mm0
	movd 48(%ebx),%mm1
	movd 48(%edx),%mm2
	pfmul %mm6,%mm5
	punpckldq 56(%ebx),%mm1
	punpckldq 56(%edx),%mm2
	pfadd %mm5,%mm0
	pfmul %mm2,%mm1
	pfadd %mm1,%mm0
	pfacc %mm1,%mm0
	pf2id %mm0,%mm0
	packssdw %mm0,%mm0
	movd %mm0,%eax
	movw %ax,0(%edi)
	incl %ebp
	movl 36(%esp),%esi
	addl $-64,%ebx
	movl $15,%ebp
	addl $4,%edi
	leal -128(%edx,%esi,8),%edx

	movl $15,%ecx
	movd (%ebx),%mm0
	movd -4(%edx),%mm1
	punpckldq 4(%ebx),%mm0
	punpckldq -8(%edx),%mm1
	.align 32
.L46:						
	movd 8(%ebx),%mm3
	movd -12(%edx),%mm4
	pfmul %mm1,%mm0
	punpckldq 12(%ebx),%mm3
	punpckldq -16(%edx),%mm4
	movd 16(%ebx),%mm5
	movd -20(%edx),%mm6
	pfmul %mm4,%mm3
	punpckldq 20(%ebx),%mm5
	punpckldq -24(%edx),%mm6
	pfadd %mm3,%mm0
	movd 24(%ebx),%mm1
	movd -28(%edx),%mm2
	pfmul %mm6,%mm5
	punpckldq 28(%ebx),%mm1	
	punpckldq -32(%edx),%mm2
	pfadd %mm5,%mm0
	movd 32(%ebx),%mm3
	movd -36(%edx),%mm4
	pfmul %mm2,%mm1
	punpckldq 36(%ebx),%mm3	
	punpckldq -40(%edx),%mm4
	pfadd %mm1,%mm0
	movd 40(%ebx),%mm5
	movd -44(%edx),%mm6
	pfmul %mm4,%mm3
	punpckldq 44(%ebx),%mm5	
	punpckldq -48(%edx),%mm6
	pfadd %mm3,%mm0
	movd 48(%ebx),%mm1
	movd -52(%edx),%mm2
	pfmul %mm6,%mm5
	punpckldq 52(%ebx),%mm1
	punpckldq -56(%edx),%mm2
	pfadd %mm0,%mm5
	movd 56(%ebx),%mm3
	movd -60(%edx),%mm4
	pfmul %mm2,%mm1
	punpckldq 60(%ebx),%mm3	
	punpckldq (%edx),%mm4
	pfadd %mm1,%mm5
	addl $-128,%edx
	addl $-64,%ebx
	movd (%ebx),%mm0
	movd -4(%edx),%mm1
	pfmul %mm4,%mm3
	punpckldq 4(%ebx),%mm0
	punpckldq -8(%edx),%mm1
	pfadd %mm5,%mm3
	pfacc %mm3,%mm3
	incl %ebp
	pf2id %mm3,%mm3
  	movd %mm3,%eax
  	negl %eax
  	movd %eax,%mm3
	packssdw %mm3,%mm3
	movd %mm3,%eax
  	movw %ax,(%edi)
	addl $4,%edi
	decl %ecx
	jnz .L46

	femms
	movl 56(%esp),%esi
	movl %ebp,%eax
	subl $-128,0(%esi)
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	addl $24,%esp
	ret
