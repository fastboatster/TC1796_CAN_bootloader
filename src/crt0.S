/*
 * crt0.S -- Startup code for GNU/TriCore applications.
 *
 * Copyright (C) 1998-2005 HighTec EDV-Systeme GmbH.
 *
 */

#ifndef __TRICORE_NAME__
#ifdef __TC13__
#define __TRICORE_NAME__ 0x13
#else
#define __TRICORE_NAME__ 0x12
#endif
#endif
/* define the Derivate Name as a hexvalue
 * this name is defined in tricore.specs for specific derivate to
 * the derivate number as a hexvalue (e.g. TC1796 => 0x1796
 * This name will be used in the memory.x Memory description to
 * to confirm that the crt0.o and the memory.x will be get from
 * same directory
 */
.global __TRICORE_DERIVATE_NAME__
.type __TRICORE_DERIVATE_NAME__,@object
.set __TRICORE_DERIVATE_NAME__,__TRICORE_NAME__


	.section ".startup_code", "awx", @progbits
	.global _start
	.type _start,@function
_start:
	.code32


    //disable watchdog:
    mov %d15, 0x08
    st.w 0xf0000024, %d15

    //set endinit bit:
    ld.w  %d0, 0xf0000020
    mov.u %d1, 0x7c
    insert %d0,%d0,%d1, 0x1, 0x7
    st.w 0xf0000020, %d0
    insert %d0,%d0, 0x3, 0x0, 0x4
    st.w 0xf0000020, %d0


	/*
	 * initialize user and interrupt stack pointers
	 */

	movh.a	%sp,hi:__USTACK			# load %sp
	lea	%sp,[%sp]lo:__USTACK
	movh	%d0,hi:__ISTACK			# load $isp
	addi	%d0,%d0,lo:__ISTACK
	mtcr	$isp,%d0
	isync

	/*
	 * initialize call depth counter
	 */

	mfcr	%d0,$psw
	andn	%d0,%d0,0x7f			# reset call depth counter
	or	%d0,%d0,0x80			# set CDE bit
	mtcr	$psw,%d0
	isync


	/*
	 * initialize context save areas (CSAs), PCXI, LCX and FCX
	 */

__init_csa:
	movh	%d0,0
	mtcr	$pcxi,%d0
	isync
    movh	%d0,hi:__CSA_BEGIN	#; %d0 = begin of CSA
	addi	%d0,%d0,lo:__CSA_BEGIN
	addi	%d0,%d0,63			#; force alignment (2^6)
	andn	%d0,%d0,63
	movh	%d2,hi:__CSA_END	#; %d2 = end of CSA
	addi	%d2,%d2,lo:__CSA_END
	andn	%d2,%d2,63			#; force alignment (2^6)
	sub	    %d2,%d2,%d0
	sh	    %d2,%d2,-6			#; %d2 = number of CSAs
	mov.a	%a3,%d0				#; %a3 = address of first CSA
	extr.u	%d0,%d0,28,4		#; %d0 = segment << 16
	sh	    %d0,%d0,16
	lea	    %a4,0				#; %a4 = previous CSA = 0
	st.a	[%a3], %a4			#; store it in 1st CSA
    mov.aa  %a4,%a3				#; %a4 = current CSA
    lea     %a3,[%a3]64			#; %a3 = %a3->nextCSA
	mov.d	%d1,%a3
	extr.u	%d1,%d1,6,16		#; get CSA index
	or	%d1,%d1,%d0			    #; add segment number
	mtcr	$lcx,%d1			#; initialize LCX
	add	%d2,%d2,-2			    #; CSAs to initialize -= 2
	mov.a	%a5, %d2			#; %a5 = loop counter
csa_loop:
	mov.d	%d1,%a4				#; %d1 = current CSA address
	extr.u	%d1,%d1,6,16		#; get CSA index
	or	%d1,%d1,%d0			    #; add segment number
	st.w	[%a3],%d1			#; store "nextCSA" pointer
    mov.aa  %a4,%a3				#; %a4 = current CSA address
    lea     %a3,[%a3]64			#; %a3 = %a3->nextCSA
	loop	%a5, csa_loop		#; repeat until done

	mov.d	%d1,%a4				#; %d1 = current CSA address
	extr.u	%d1,%d1,6,16		#; get CSA index
	or	%d1,%d1,%d0			    #; add segment number
	mtcr	$fcx,%d1			#; initialize FCX
	isync

	// SBRAM unlock attempt
	movh.a %a2,0xf801
	lea %a2,[%a2]0x1e0
	mov %d12, 0x2
	st.w [%a2], %d12
	mov %d12, 0x6
	st.w [%a2], %d12
	mov %d12, 0xe
	st.w [%a2], %d12
    j main

