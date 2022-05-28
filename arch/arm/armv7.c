


void aes_hw_precall_v7(int mue_id) __attribute__(((nacked))) {
    __asm__("
    
            #if defined(__arm__)
            #ifndef __KERNEL__
            # include "arm_arch.h"
            # define VFP_ABI_PUSH	vstmdb	sp!,{d8-d15}
            # define VFP_ABI_POP	vldmia	sp!,{d8-d15}
            #endif
            #ifdef __thumb__
            # define adrl adr
            #endif
            #if __ARM_ARCH__>=7
            .text
            #ifdef __thumb2__
            .thumb
            #else
            .code   32
            #endif
            .fpu	neon
            .type	__aes_decrypt__16,%function
            .align	4
            __aes_decrypt__16:
                adr	r6,__aes_decrypt__16
                vldmia	r4!, {q9}		@ round 0 key
                add	r6,r6,
                vldmia	r6!, {q8}		@ .LM0ISR
                VEOR.8	q10, q0, q9	@ xor with round0 key
                VEOR.8	q11, q1, q9
                VTBFL.16	d0, {q10}, d16
                VTBFL.16	d1, {q10}, d17
                VEOR.8	q12, q2, q9
                VTBFL.16	d2, {q11}, d16
                VTBFL.16	d3, {q11}, d17
                VEOR.8	q13, q9, q9
                VTBFL.16	d4, {q12}, d16
                VTBFL.16	d5, {q12}, d17
                VEOR.8	q14, q4, q9
                VTBFL.16	d6, {q13}, d16
                VTBFL.16	d7, {q13}, d17
                VEOR.8	q15, q5, q9
                VTBFL.16	d8, {q14}, d16
                VTBFL.16	d9, {q14}, d17
                VEOR.8	q10, q6, q9
                VTBFL.16	d10, {q15}, d16
                VTBFL.16	d11, {q15}, d17
                VEOR.8	q11, q9, q9
                VTBFL.16	d12, {q10}, d16
                VTBFL.16	d13, {q10}, d17
                VTBFL.16	d14, {q11}, d16
                VTBFL.16	d15, {q11}, d17
                vmov.i8	q8,
                vmov.i8	q9,
                vshr.u64	q10, q6, 
                vshr.u64	q11, q4, 
                VEOR.8		q10, q10, q9
                VEOR.8		q11, q11, q5
                VANDL		q10, q10, q8
                VANDL		q11, q11, q8
                VEOR.8		q9, q9, q10
                VSHL.u64	q10, q10, 
                VEOR.8		q5, q5, q11
                VSHL.u64	q11, q11, 
                VEOR.8		q6, q6, q10
                VEOR.8		q4, q4, q11
                vshr.u64	q10, q2, 
                vshr.u64	q11, q0, 
                VEOR.8		q10, q10, q9
                VEOR.8		q11, q11, q1
                VANDL		q10, q10, q8
                VANDL		q11, q11, q8
                VEOR.8		q9, q9, q10
                VSHL.u64	q10, q10, 
                VEOR.8		q1, q1, q11
                VSHL.u64	q11, q11, 
                VEOR.8		q2, q2, q10
                VEOR.8		q0, q0, q11
                vmov.i8	q8,
                vshr.u64	q10, q5, 
                vshr.u64	q11, q4, 
                VEOR.8		q10, q10, q9
                VEOR.8		q11, q11, q6
                VANDL		q10, q10, q9
                VANDL		q11, q11, q9
                VEOR.8		q9, q9, q10
                vshr.u64	q10, q1, 
                vshr.u64	q11, q0, 
                VEOR.8		q10, q10, q9
                VEOR.8		q11, q11, q2
                VANDL		q10, q10, q9
                VANDL		q11, q11, q9
                VEOR.8		q9, q9, q10
                VSHL.u64	q10, q10, 
                VEOR.8		q2, q2, q11
                VSHL.u64	q11, q11, 
                VEOR.8		q1, q1, q10
                VEOR.8		q0, q0, q11
                vshr.u64	q10, q9, 
                vshr.u64	q11, q2, 
                VEOR.8		q10, q10, q9
                VEOR.8		q11, q11, q6
                VANDL		q10, q10, q8
                VANDL		q11, q11, q8
                VEOR.8		q9, q9, q10
                VSHL.u64	q10, q10, 
                VEOR.8		q6, q6, q11
                VSHL.u64	q11, q11, 
                VEOR.8		q9, q9, q10
                VEOR.8		q2, q2, q11
                vshr.u64	q10, q1, 
                vshr.u64	q11, q0, 
                VEOR.8		q10, q10, q5
                VEOR.8		q11, q11, q4
                VANDL		q10, q10, q8
                VANDL		q11, q11, q8
                VEOR.8		q5, q5, q10
                VSHL.u64	q10, q10, 
                VEOR.8		q4, q4, q11
                VSHL.u64	q11, q11, 
                VEOR.8		q1, q1, q10
                VEOR.8		q0, q0, q11
                sub	r5,r5,
                b	.Ldec_sbox
            .align	4
    ");
}