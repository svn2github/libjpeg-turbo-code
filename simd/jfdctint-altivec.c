/*
 * AltiVec optimizations for libjpeg-turbo
 *
 * Copyright (C) 2014, D. R. Commander.
 * All rights reserved.
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/* SLOW INTEGER FORWARD DCT */

#include "jsimd_altivec.h"


#define F_0_298 2446   /* FIX(0.298631336) */
#define F_0_390 3196   /* FIX(0.390180644) */
#define F_0_541 4433   /* FIX(0.541196100) */
#define F_0_765 6270   /* FIX(0.765366865) */
#define F_0_899 7373   /* FIX(0.899976223) */
#define F_1_175 9633   /* FIX(1.175875602) */
#define F_1_501 12299  /* FIX(1.501321110) */
#define F_1_847 15137  /* FIX(1.847759065) */
#define F_1_961 16069  /* FIX(1.961570560) */
#define F_2_053 16819  /* FIX(2.053119869) */
#define F_2_562 20995  /* FIX(2.562915447) */
#define F_3_072 25172  /* FIX(3.072711026) */

#define CONST_BITS 13
#define PASS1_BITS 2
#define DESCALE_P1 (CONST_BITS - PASS1_BITS)
#define DESCALE_P2 (CONST_BITS + PASS1_BITS)


#define DO_FDCT_COMMON(PASS)  \
{  \
  tmp1312l = vec_mergeh(tmp13, tmp12);  \
  tmp1312h = vec_mergel(tmp13, tmp12);  \
  \
  out2l = vec_msums(tmp1312l, pw_f130_f054, pd_descale_p##PASS);  \
  out2h = vec_msums(tmp1312h, pw_f130_f054, pd_descale_p##PASS);  \
  out6l = vec_msums(tmp1312l, pw_f054_mf130, pd_descale_p##PASS);  \
  out6h = vec_msums(tmp1312h, pw_f054_mf130, pd_descale_p##PASS);  \
  \
  out2l = vec_sra(out2l, descale_p##PASS);  \
  out2h = vec_sra(out2h, descale_p##PASS);  \
  out6l = vec_sra(out6l, descale_p##PASS);  \
  out6h = vec_sra(out6h, descale_p##PASS);  \
  \
  out2 = vec_pack(out2l, out2h);  \
  out6 = vec_pack(out6l, out6h);  \
  \
  /* Odd part */  \
  \
  z3 = vec_add(tmp4, tmp6);  \
  z4 = vec_add(tmp5, tmp7);  \
  \
  z34l = vec_mergeh(z3, z4);  \
  z34h = vec_mergel(z3, z4);  \
  \
  z3l = vec_msums(z34l, pw_mf078_f117, pd_descale_p##PASS);  \
  z3h = vec_msums(z34h, pw_mf078_f117, pd_descale_p##PASS);  \
  z4l = vec_msums(z34l, pw_f117_f078, pd_descale_p##PASS);  \
  z4h = vec_msums(z34h, pw_f117_f078, pd_descale_p##PASS);  \
  \
  tmp47l = vec_mergeh(tmp4, tmp7);  \
  tmp47h = vec_mergel(tmp4, tmp7);  \
  \
  out7l = vec_msums(tmp47l, pw_mf060_mf089, z3l);  \
  out7h = vec_msums(tmp47h, pw_mf060_mf089, z3h);  \
  out1l = vec_msums(tmp47l, pw_mf089_f060, z4l);  \
  out1h = vec_msums(tmp47h, pw_mf089_f060, z4h);  \
  \
  out7l = vec_sra(out7l, descale_p##PASS);  \
  out7h = vec_sra(out7h, descale_p##PASS);  \
  out1l = vec_sra(out1l, descale_p##PASS);  \
  out1h = vec_sra(out1h, descale_p##PASS);  \
  \
  out7 = vec_pack(out7l, out7h);  \
  out1 = vec_pack(out1l, out1h);  \
  \
  tmp56l = vec_mergeh(tmp5, tmp6);  \
  tmp56h = vec_mergel(tmp5, tmp6);  \
  \
  out5l = vec_msums(tmp56l, pw_mf050_mf256, z4l);  \
  out5h = vec_msums(tmp56h, pw_mf050_mf256, z4h);  \
  out3l = vec_msums(tmp56l, pw_mf256_f050, z3l);  \
  out3h = vec_msums(tmp56h, pw_mf256_f050, z3h);  \
  \
  out5l = vec_sra(out5l, descale_p##PASS);  \
  out5h = vec_sra(out5h, descale_p##PASS);  \
  out3l = vec_sra(out3l, descale_p##PASS);  \
  out3h = vec_sra(out3h, descale_p##PASS);  \
  \
  out5 = vec_pack(out5l, out5h);  \
  out3 = vec_pack(out3l, out3h);  \
}

#define DO_FDCT_ROWS()  \
{  \
  /* Even part */  \
  \
  tmp10 = vec_add(tmp0, tmp3);  \
  tmp13 = vec_sub(tmp0, tmp3);  \
  tmp11 = vec_add(tmp1, tmp2);  \
  tmp12 = vec_sub(tmp1, tmp2);  \
  \
  out0  = vec_add(tmp10, tmp11);  \
  out0  = vec_sl(out0, pass1_bits);  \
  out4  = vec_sub(tmp10, tmp11);  \
  out4  = vec_sl(out4, pass1_bits);  \
  \
  DO_FDCT_COMMON(1);  \
}

#define DO_FDCT_COLS()  \
{  \
  /* Even part */  \
  \
  tmp10 = vec_add(tmp0, tmp3);  \
  tmp13 = vec_sub(tmp0, tmp3);  \
  tmp11 = vec_add(tmp1, tmp2);  \
  tmp12 = vec_sub(tmp1, tmp2);  \
  \
  out0  = vec_add(tmp10, tmp11);  \
  out0  = vec_add(out0, pw_descale_p2x);  \
  out0  = vec_sra(out0, pass1_bits);  \
  out4  = vec_sub(tmp10, tmp11);  \
  out4  = vec_add(out4, pw_descale_p2x);  \
  out4  = vec_sra(out4, pass1_bits);  \
  \
  DO_FDCT_COMMON(2);  \
}


void
jsimd_fdct_islow_altivec (DCTELEM *data)
{
  __vector short row0, row1, row2, row3, row4, row5, row6, row7,
    col0, col1, col2, col3, col4, col5, col6, col7,
    tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp10, tmp11, tmp12, tmp13,
    tmp47l, tmp47h, tmp56l, tmp56h, tmp1312l, tmp1312h,
    z3, z4, z34l, z34h,
    out0, out1, out2, out3, out4, out5, out6, out7;
  __vector int z3l, z3h, z4l, z4h,
    out1l, out1h, out2l, out2h, out3l, out3h, out5l, out5h, out6l, out6h,
    out7l, out7h;

  /* Constants */
  __vector short
    pw_f130_f054 = { __4X2(F_0_541 + F_0_765, F_0_541) },
    pw_f054_mf130 = { __4X2(F_0_541, F_0_541 - F_1_847) },
    pw_mf078_f117 = { __4X2(F_1_175 - F_1_961, F_1_175) },
    pw_f117_f078 = { __4X2(F_1_175, F_1_175 - F_0_390) },
    pw_mf060_mf089 = { __4X2(F_0_298 - F_0_899, -F_0_899) },
    pw_mf089_f060 = { __4X2(-F_0_899, F_1_501 - F_0_899) },
    pw_mf050_mf256 = { __4X2(F_2_053 - F_2_562, -F_2_562) },
    pw_mf256_f050 = { __4X2(-F_2_562, F_3_072 - F_2_562) },
    pw_descale_p2x = { __8X(1 << (PASS1_BITS - 1)) };
  __vector unsigned short pass1_bits = { __8X(PASS1_BITS) };
  __vector int pd_descale_p1 = { __4X(1 << (DESCALE_P1 - 1)) },
    pd_descale_p2 = { __4X(1 << (DESCALE_P2 - 1)) };
  __vector unsigned int descale_p1 = { __4X(DESCALE_P1) },
    descale_p2 = { __4X(DESCALE_P2) };

  /* Pass 1: process rows */

  row0 = *(__vector short *)&data[0];
  row1 = *(__vector short *)&data[8];
  row2 = *(__vector short *)&data[16];
  row3 = *(__vector short *)&data[24];
  row4 = *(__vector short *)&data[32];
  row5 = *(__vector short *)&data[40];
  row6 = *(__vector short *)&data[48];
  row7 = *(__vector short *)&data[56];

  TRANSPOSE(row, col);

  tmp0 = vec_add(col0, col7);
  tmp7 = vec_sub(col0, col7);
  tmp1 = vec_add(col1, col6);
  tmp6 = vec_sub(col1, col6);
  tmp2 = vec_add(col2, col5);
  tmp5 = vec_sub(col2, col5);
  tmp3 = vec_add(col3, col4);
  tmp4 = vec_sub(col3, col4);

  DO_FDCT_ROWS();

  /* Pass 2: process columns */

  TRANSPOSE(out, row);

  tmp0 = vec_add(row0, row7);
  tmp7 = vec_sub(row0, row7);
  tmp1 = vec_add(row1, row6);
  tmp6 = vec_sub(row1, row6);
  tmp2 = vec_add(row2, row5);
  tmp5 = vec_sub(row2, row5);
  tmp3 = vec_add(row3, row4);
  tmp4 = vec_sub(row3, row4);

  DO_FDCT_COLS();

  *(__vector short *)&data[0] = out0;
  *(__vector short *)&data[8] = out1;
  *(__vector short *)&data[16] = out2;
  *(__vector short *)&data[24] = out3;
  *(__vector short *)&data[32] = out4;
  *(__vector short *)&data[40] = out5;
  *(__vector short *)&data[48] = out6;
  *(__vector short *)&data[56] = out7;
}
