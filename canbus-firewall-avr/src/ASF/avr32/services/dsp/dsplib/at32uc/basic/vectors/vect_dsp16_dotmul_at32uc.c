/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit vector dot multiplication function optimized for the avr32uc3
 *
 * This file contains the code of the vector dot multiplication.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "dsp.h"
#include "preprocessor.h"

#if !defined(FORCE_ALL_GENERICS) && \
    !defined(FORCE_GENERIC_VECT16_DOTMUL) && \
    defined(TARGET_SPECIFIC_VECT16_DOTMUL)

#if __GNUC__
#  define DSP16_DOTMUL_END_KERNEL_X_FCT(x_num, data)  __attribute__((__naked__)) DSP16_DOTMUL_END_KERNEL_X_FCT__(x_num, data)
#elif __ICCAVR32__
#  define DSP16_DOTMUL_END_KERNEL_X_FCT(x_num, data)  DSP16_DOTMUL_END_KERNEL_X_FCT__(x_num, data)
#endif


#if __GNUC__
#  define ASM_INSTRUCT_COMPACKED(str) str
#  define ASM_INSTRUCT_EXTENDED(str)  str
#elif __ICCAVR32__
#  define ASM_INSTRUCT_COMPACKED(str) str":C"
#  define ASM_INSTRUCT_EXTENDED(str)  str":E"
#endif



/*********************************************************************************************
 * Macro name: DSP16_DOTMULTIPLICATION_X
 * Used registers:
 *   r0, r1, r2, r3
 * Pre:
 * Description:
 *   vect1(0) = (vect2(0) * vect3(0)) >> DSP16_QB
 *   vect1(1) = (vect2(1) * vect3(1)) >> DSP16_QB
 *   ... X times ...
 *   where X is the number of iteration
 *********************************************************************************************/
#define DSP16_DOTMULTIPLICATION_0(r_vect1, r_vect2, r_vect3)

#define DSP16_DOTMULTIPLICATION_1(r_vect1, r_vect2, r_vect3) \
        "ld.sh    r2, "ASTRINGZ(r_vect2)"[0x0]\n\t" \
        "ld.sh    r3, "ASTRINGZ(r_vect3)"[0x0]\n\t" \
\
        "mul      r1, r2, r3\n\t" \
        "lsr      r1, "ASTRINGZ(DSP16_QB)"\n\t" \
        "st.h     "ASTRINGZ(r_vect1)"[0x0], r1\n\t"

#define DSP16_DOTMULTIPLICATION_2(r_vect1, r_vect2, r_vect3) \
        "ld.w     r2, "ASTRINGZ(r_vect2)"[0x0]\n\t" \
        "ld.w     r3, "ASTRINGZ(r_vect3)"[0x0]\n\t" \
\
        "mulhh.w  r1, r2:b, r3:b\n\t" \
        "lsr      r1, "ASTRINGZ(DSP16_QB)"\n\t" \
        "mulhh.w  r0, r2:t, r3:t\n\t" \
        "lsr      r0, "ASTRINGZ(DSP16_QB)"\n\t" \
        "sthh.w   "ASTRINGZ(r_vect1)"[0x0], r0:b, r1:b\n\t"

#define DSP16_DOTMULTIPLICATION_3(r_vect1, r_vect2, r_vect3) \
        "ld.w     r2, "ASTRINGZ(r_vect2)"[0x0]\n\t" \
        "ld.w     r3, "ASTRINGZ(r_vect3)"[0x0]\n\t" \
\
        "mulhh.w  r1, r2:b, r3:b\n\t" \
        "lsr      r1, "ASTRINGZ(DSP16_QB)"\n\t" \
        "mulhh.w  r0, r2:t, r3:t\n\t" \
        "lsr      r0, "ASTRINGZ(DSP16_QB)"\n\t" \
        "sthh.w   "ASTRINGZ(r_vect1)"[0x0], r0:b, r1:b\n\t" \
\
        "ld.sh    r2, "ASTRINGZ(r_vect2)"[0x4]\n\t" \
        "ld.sh    r3, "ASTRINGZ(r_vect3)"[0x4]\n\t" \
\
        "mul      r1, r2, r3\n\t" \
        "lsr      r1, "ASTRINGZ(DSP16_QB)"\n\t" \
        "st.h     "ASTRINGZ(r_vect1)"[0x4], r1\n\t"
/*********************************************************************************************/


/*********************************************************************************************
 * This function is the ending function of the dot multiplication. It is used to multiply the last items of a vector.
 *********************************************************************************************/
#define DSP16_DOTMUL_END_KERNEL_X_FCT__(x_num, data) \
static void TPASTE2(dsp16_vect_dotmul_end_kernel_x, x_num)(dsp16_t *vect1, dsp16_t *vect2, dsp16_t *vect3) \
{ \
  __asm__ __volatile__ ( \
    "pushm  r0-r3, lr\n\t" \
    TPASTE2(DSP16_DOTMULTIPLICATION_, x_num)(r12, r11, r10) \
    "popm  r0-r3, pc\n\t" \
  ); \
}
/*********************************************************************************************/



/*********************************************************************************************
 * Algorithm:
 * for(n=0; n<size-4; n+=4)
 * {
 *   vect1[n] = (vect2[n] * vect3[n]) >> DSP16_QB;
 *   vect1[n+1] = (vect2[n+1] * vect3[n+1]) >> DSP16_QB;
 *   vect1[n+2] = (vect2[n+2] * vect3[n+2]) >> DSP16_QB;
 *   vect1[n+3] = (vect2[n+3] * vect3[n+3]) >> DSP16_QB;
 * }
 *********************************************************************************************/
//! avr32-uc3 16 bit version
#if __GNUC__
__attribute__((__naked__))
__attribute__((__noinline__))
#elif __ICCAVR32__
# pragma shadow_registers=full
# pragma optimize=none no_inline
#endif
static int dsp16_vect_dotmul_kernel_ext(dsp16_t *vect1, dsp16_t *vect2, dsp16_t *vect3, int size)
{
  __asm__ __volatile__ ( \
      "pushm    r0-r7, lr\n\t" \
\
      "mov      lr, 0\n\t" \
      "sub      r9, 3\n\t" \
\
      "cp.h     lr, r9\n\t" \
      ASM_INSTRUCT_COMPACKED("brge __dsp16_dotmul_ext_end_loop")"\n" \
\
    "__dsp16_dotmul_ext_loop:\n\t" \
\
      "ld.d     r0, r11[lr << 1]\n\t" \
      "ld.d     r2, r10[lr << 1]\n\t" \
\
      "mulhh.w  r4, r1:b, r3:b\n\t" \
      "lsr      r4, "ASTRINGZ(DSP16_QB)"\n\t" \
      "mulhh.w  r5, r1:t, r3:t\n\t" \
      "lsr      r5, "ASTRINGZ(DSP16_QB)"\n\t" \
      "sthh.w   r12[lr << 1], r5:b, r4:b\n\t" \
\
      "sub      lr, -2\n\t" \
      "mulhh.w  r4, r0:b, r2:b\n\t" \
      "lsr      r4, "ASTRINGZ(DSP16_QB)"\n\t" \
      "mulhh.w  r5, r0:t, r2:t\n\t" \
      "lsr      r5, "ASTRINGZ(DSP16_QB)"\n\t" \
      "sthh.w   r12[lr << 1], r5:b, r4:b\n\t" \
\
      "sub      lr, -2\n\t" \
\
      "cp.h     lr, r9\n\t" \
      ASM_INSTRUCT_COMPACKED("brlt __dsp16_dotmul_ext_loop")"\n" \
\
    "__dsp16_dotmul_ext_end_loop:\n\t" \
\
      "mov      r12, lr\n\t" \
      "popm     r0-r7, pc\n\t" \
    ); \

  return 0;
}
/*********************************************************************************************/

DSP16_DOTMUL_END_KERNEL_X_FCT(0, "")
DSP16_DOTMUL_END_KERNEL_X_FCT(1, "")
DSP16_DOTMUL_END_KERNEL_X_FCT(2, "")
DSP16_DOTMUL_END_KERNEL_X_FCT(3, "")

void dsp16_vect_dotmul(dsp16_t *vect1, dsp16_t *vect2, dsp16_t *vect3, int size)
{
  typedef void (*dotmul_end_kernel_opti_t)(dsp16_t *, dsp16_t *, dsp16_t *);
  static const dotmul_end_kernel_opti_t dotmul_end_kernel_opti[4] = {
    dsp16_vect_dotmul_end_kernel_x0,
    dsp16_vect_dotmul_end_kernel_x1,
    dsp16_vect_dotmul_end_kernel_x2,
    dsp16_vect_dotmul_end_kernel_x3
  };
  int n;

  n = dsp16_vect_dotmul_kernel_ext(vect1, vect2, vect3, size);

  // Jump on different functions depending on the length of the vectors to compute
  dotmul_end_kernel_opti[size&0x3](&vect1[n], &vect2[n], &vect3[n]);
}

#endif
