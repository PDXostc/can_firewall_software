/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit ramp signal generator functions for the AVR32 UC3
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

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN16_RAMP) || \
    !defined(TARGET_SPECIFIC_GEN16_RAMP)

// y = (0:L-1)*m + b
void dsp16_gen_ramp(dsp16_t *vect1, int size, dsp16_t increment)
{
  int i = 0;
  dsp16_t t, limit_t;

  // Limit of the signal
  limit_t = DSP16_Q(1.) - increment;
  if ((((S32) increment) * ((S32) size)) < ((S32) limit_t))
    limit_t = increment * size;

  // Ramp loop
  for(t = DSP16_Q(0.); t < limit_t; t += increment)
    vect1[i++] = t;
  // Filling loop
  for(; i<size; i++)
    vect1[i] = DSP16_Q(1.);
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN32_RAMP) || \
    !defined(TARGET_SPECIFIC_GEN32_RAMP)

void dsp32_gen_ramp(dsp32_t *vect1, int size, dsp32_t increment)
{
  int i = 0;
  dsp32_t t, limit_t;

  // Limit of the signal
  limit_t = DSP32_Q(1.) - increment;
  if ((((S64) increment) * ((S64) size)) < ((S64) limit_t))
    limit_t = increment * size;

  // Ramp loop
  for(t = DSP32_Q(0.); t < limit_t; t += increment)
    vect1[i++] = t;
  // Filling loop
  for(; i<size; i++)
    vect1[i] = DSP32_Q(1.);
}

#endif
