/*
 * HEVC video decoder
 *
 * Copyright (C) 2012 - 2013 Guillaume Martres
 * Copyright (C) 2013 - 2014 Pierre-Edouard Lepere
 *
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVCODEC_X86_HEVCDSP_H
#define AVCODEC_X86_HEVCDSP_H

struct SAOParams;
struct AVFrame;
struct UpsamplInf;
struct HEVCWindow;

//#define OPTI_ASM

#define PEL_LINK2(dst, idx1, idx2, idx3, name, D, opt)                                    \
dst           [idx1][idx2][idx3] = ff_hevc_put_hevc_       ## name ## _ ## D ## _ ## opt; \
dst ## _bi    [idx1][idx2][idx3] = ff_hevc_put_hevc_bi_    ## name ## _ ## D ## _ ## opt; \
dst ## _uni   [idx1][idx2][idx3] = ff_hevc_put_hevc_uni_   ## name ## _ ## D ## _ ## opt; \
dst ## _uni_w [idx1][idx2][idx3] = ff_hevc_put_hevc_uni_w_ ## name ## _ ## D ## _ ## opt; \
dst ## _bi_w  [idx1][idx2][idx3] = ff_hevc_put_hevc_bi_w_  ## name ## _ ## D ## _ ## opt

//#ifdef OPTI_ASM
#define PEL_LINK(dst, idx1, idx2, idx3, name, D, opt)                       \
PEL_LINK2(dst, idx1, idx2, idx3, name, D, opt)
//#else
//#define PEL_LINK(dst, idx1, idx2, idx3, name, D, opt) \
//PEL_LINK2(dst, idx1, idx2, idx3, name, D, sse)
//#endif

#define PEL_PROTOTYPE2(name, D, opt) \
void ff_hevc_put_hevc_       ## name ## _ ## D ## _##opt(int16_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width); \
void ff_hevc_put_hevc_bi_    ## name ## _ ## D ## _##opt(uint8_t *_dst, ptrdiff_t _dststride, uint8_t *_src, ptrdiff_t _srcstride, int16_t *src2, ptrdiff_t src2stride, int height, intptr_t mx, intptr_t my, int width); \
void ff_hevc_put_hevc_uni_   ## name ## _ ## D ## _##opt(uint8_t *_dst, ptrdiff_t _dststride, uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width); \
void ff_hevc_put_hevc_uni_w_ ## name ## _ ## D ## _##opt(uint8_t *_dst, ptrdiff_t _dststride, uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width); \
void ff_hevc_put_hevc_bi_w_  ## name ## _ ## D ## _##opt(uint8_t *_dst, ptrdiff_t _dststride, uint8_t *_src, ptrdiff_t _srcstride, int16_t *src2, ptrdiff_t src2stride, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width)


#define WEIGHTING_PROTOTYPE2(width, bitd, opt) \
void ff_hevc_put_hevc_uni_w ## width ## _ ## bitd ##_## opt(uint8_t *dst, ptrdiff_t dststride, int16_t *_src, ptrdiff_t _srcstride, int height, int denom,  int _wx, int _ox); \
void ff_hevc_put_hevc_bi_w  ## width ## _ ## bitd ##_## opt(uint8_t *dst, ptrdiff_t dststride, int16_t *_src, ptrdiff_t _srcstride, int16_t *_src2, ptrdiff_t _src2stride, int height, int denom,  int _wx0,  int _wx1, int _ox0, int _ox1)

//#ifdef OPTI_ASM
#define WEIGHTING_PROTOTYPE(width, bitd, opt) \
		WEIGHTING_PROTOTYPE2(width, bitd, opt)
//#else
//#define WEIGHTING_PROTOTYPE(width, bitd, opt) \
//		WEIGHTING_PROTOTYPE2(width, bitd, sse)
//#endif

//#ifdef OPTI_ASM
#define PEL_PROTOTYPE(name, D, opt) \
PEL_PROTOTYPE2(name, D, opt)
//#else
//#define PEL_PROTOTYPE(name, D, opt) \
//PEL_PROTOTYPE2(name, D, sse)
//#endif



///////////////////////////////////////////////////////////////////////////////
//IDCT functions
///////////////////////////////////////////////////////////////////////////////
void ff_hevc_transform_skip_8_sse(uint8_t *_dst, int16_t *coeffs, ptrdiff_t _stride);

void ff_hevc_transform_4x4_luma_8_sse2(int16_t *coeffs);
void ff_hevc_transform_4x4_luma_10_sse2(int16_t *coeffs);
void ff_hevc_transform_4x4_luma_12_sse2(int16_t *coeffs);

#define IDCT_FUNC(s, b)                                                       \
void ff_hevc_transform_ ## s ## x ## s ##_## b ##_sse2(int16_t *coeffs, int col_limit);

IDCT_FUNC(4, 8)
IDCT_FUNC(4, 10)
IDCT_FUNC(4, 12)

IDCT_FUNC(8, 8)
IDCT_FUNC(8, 10)
IDCT_FUNC(8, 12)

IDCT_FUNC(16, 8)
IDCT_FUNC(16, 10)
IDCT_FUNC(16, 12)

IDCT_FUNC(32, 8)
IDCT_FUNC(32, 10)
IDCT_FUNC(32, 12)

#define TRANSFORM_ADD_FUNC_MMXEXT(s, b) void ff_hevc_transform_add ## s ## _ ## b ##_mmxext           \
		(uint8_t *dst, int16_t *coeffs, ptrdiff_t stride);

#define TRANSFORM_ADD_FUNC_SSE2(s, b)   void ff_hevc_transform_add   ## s ## _ ## b ##_sse2           \
		(uint8_t *dst, int16_t *coeffs, ptrdiff_t stride);

#define TRANSFORM_ADD_FUNC_AVX(s, b)    void ff_hevc_transform_add    ## s ## _ ## b ##_avx           \
		(uint8_t *dst, int16_t *coeffs, ptrdiff_t stride);

#define TRANSFORM_ADD_FUNC_AVX2(s, b)   void ff_hevc_transform_add  ## s ## _ ## b ##_avx2            \
		(uint8_t *dst, int16_t *coeffs, ptrdiff_t stride);

#define TRANSFORM_ADD2_FUNC_SSE2(s, b)  void ff_hevc_transform_     ## s ## x ## s ##_add_## b ##_sse2\
		(uint8_t *dst, int16_t *coeffs, ptrdiff_t stride);

TRANSFORM_ADD_FUNC_MMXEXT(4,8)
TRANSFORM_ADD_FUNC_SSE2(8,8)
TRANSFORM_ADD_FUNC_SSE2(16,8)
TRANSFORM_ADD_FUNC_SSE2(32,8)

TRANSFORM_ADD_FUNC_MMXEXT(4,10)
TRANSFORM_ADD_FUNC_SSE2(8,10)
TRANSFORM_ADD_FUNC_SSE2(16,10)
TRANSFORM_ADD_FUNC_SSE2(32,10)

TRANSFORM_ADD2_FUNC_SSE2(4,8)
TRANSFORM_ADD2_FUNC_SSE2(8,8)
TRANSFORM_ADD2_FUNC_SSE2(16,8)
TRANSFORM_ADD2_FUNC_SSE2(32,8)

TRANSFORM_ADD2_FUNC_SSE2(4,10)
TRANSFORM_ADD2_FUNC_SSE2(8,10)
TRANSFORM_ADD2_FUNC_SSE2(16,10)
TRANSFORM_ADD2_FUNC_SSE2(32,10)

TRANSFORM_ADD2_FUNC_SSE2(4,12)
TRANSFORM_ADD2_FUNC_SSE2(8,12)
TRANSFORM_ADD2_FUNC_SSE2(16,12)
TRANSFORM_ADD2_FUNC_SSE2(32,12)

TRANSFORM_ADD_FUNC_AVX(8,10)
TRANSFORM_ADD_FUNC_AVX(16,10)
TRANSFORM_ADD_FUNC_AVX(32,10)

TRANSFORM_ADD_FUNC_AVX2(16,10)
TRANSFORM_ADD_FUNC_AVX2(32,10)

void ff_hevc_transform_4x4_add_8_sse4(uint8_t *dst, int16_t *_coeffs, ptrdiff_t _stride);
void ff_hevc_transform_8x8_add_8_sse4(uint8_t *dst, int16_t *_coeffs, ptrdiff_t _stride);
void ff_hevc_transform_16x16_add_8_sse4(uint8_t *dst, int16_t *_coeffs, ptrdiff_t _stride);
void ff_hevc_transform_32x32_add_8_sse4(uint8_t *dst, int16_t *_coeffs, ptrdiff_t _stride);

///////////////////////////////////////////////////////////////////////////////
// MC functions
///////////////////////////////////////////////////////////////////////////////
#define EPEL_PROTOTYPES(fname, bitd, opt) \
        PEL_PROTOTYPE(fname##4,  bitd, opt); \
        PEL_PROTOTYPE(fname##6,  bitd, opt); \
        PEL_PROTOTYPE(fname##8,  bitd, opt); \
        PEL_PROTOTYPE(fname##12, bitd, opt); \
        PEL_PROTOTYPE(fname##16, bitd, opt); \
        PEL_PROTOTYPE(fname##24, bitd, opt); \
        PEL_PROTOTYPE(fname##32, bitd, opt); \
        PEL_PROTOTYPE(fname##48, bitd, opt); \
        PEL_PROTOTYPE(fname##64, bitd, opt)

#define QPEL_PROTOTYPES(fname, bitd, opt) \
        PEL_PROTOTYPE(fname##4,  bitd, opt); \
        PEL_PROTOTYPE(fname##8,  bitd, opt); \
        PEL_PROTOTYPE(fname##12, bitd, opt); \
        PEL_PROTOTYPE(fname##16, bitd, opt); \
        PEL_PROTOTYPE(fname##24, bitd, opt); \
        PEL_PROTOTYPE(fname##32, bitd, opt); \
        PEL_PROTOTYPE(fname##48, bitd, opt); \
        PEL_PROTOTYPE(fname##64, bitd, opt)

#define WEIGHTING_PROTOTYPES(bitd, opt) \
        WEIGHTING_PROTOTYPE(2, bitd, opt); \
        WEIGHTING_PROTOTYPE(4, bitd, opt); \
        WEIGHTING_PROTOTYPE(6, bitd, opt); \
        WEIGHTING_PROTOTYPE(8, bitd, opt); \
        WEIGHTING_PROTOTYPE(12, bitd, opt); \
        WEIGHTING_PROTOTYPE(16, bitd, opt); \
        WEIGHTING_PROTOTYPE(24, bitd, opt); \
        WEIGHTING_PROTOTYPE(32, bitd, opt); \
        WEIGHTING_PROTOTYPE(48, bitd, opt); \
        WEIGHTING_PROTOTYPE(64, bitd, opt)

///////////////////////////////////////////////////////////////////////////////
// QPEL_PIXELS EPEL_PIXELS
///////////////////////////////////////////////////////////////////////////////
EPEL_PROTOTYPES(pel_pixels ,  8, avx2_);
EPEL_PROTOTYPES(pel_pixels , 10, avx2_);
EPEL_PROTOTYPES(pel_pixels , 12, avx2_);

EPEL_PROTOTYPES(pel_pixels ,  8, sse4_);
EPEL_PROTOTYPES(pel_pixels , 10, sse4_);
EPEL_PROTOTYPES(pel_pixels , 12, sse4_);
/*
void ff_hevc_put_hevc_pel_pixels16_8_avx2(int16_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width);
void ff_hevc_put_hevc_pel_pixels24_8_avx2(int16_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width);
void ff_hevc_put_hevc_pel_pixels32_8_avx2(int16_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width);
void ff_hevc_put_hevc_pel_pixels48_8_avx2(int16_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width);
void ff_hevc_put_hevc_pel_pixels64_8_avx2(int16_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width);

void ff_hevc_put_hevc_pel_pixels16_10_avx2(int16_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width);
void ff_hevc_put_hevc_pel_pixels24_10_avx2(int16_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width);
void ff_hevc_put_hevc_pel_pixels32_10_avx2(int16_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width);
void ff_hevc_put_hevc_pel_pixels48_10_avx2(int16_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width);
void ff_hevc_put_hevc_pel_pixels64_10_avx2(int16_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width);



void ff_hevc_put_hevc_uni_pel_pixels32_8_avx2(uint8_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width);
void ff_hevc_put_hevc_uni_pel_pixels48_8_avx2(uint8_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width);
void ff_hevc_put_hevc_uni_pel_pixels64_8_avx2(uint8_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width);
void ff_hevc_put_hevc_uni_pel_pixels96_8_avx2(uint8_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width); //used for 10bit
void ff_hevc_put_hevc_uni_pel_pixels128_8_avx2(uint8_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width);//used for 10bit


void ff_hevc_put_hevc_bi_pel_pixels16_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, uint8_t *_src, ptrdiff_t _srcstride, int16_t *src2, ptrdiff_t src2stride, int height, intptr_t mx, intptr_t my, int width);
void ff_hevc_put_hevc_bi_pel_pixels24_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, uint8_t *_src, ptrdiff_t _srcstride, int16_t *src2, ptrdiff_t src2stride, int height, intptr_t mx, intptr_t my, int width);
void ff_hevc_put_hevc_bi_pel_pixels32_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, uint8_t *_src, ptrdiff_t _srcstride, int16_t *src2, ptrdiff_t src2stride, int height, intptr_t mx, intptr_t my, int width);
void ff_hevc_put_hevc_bi_pel_pixels48_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, uint8_t *_src, ptrdiff_t _srcstride, int16_t *src2, ptrdiff_t src2stride, int height, intptr_t mx, intptr_t my, int width);
void ff_hevc_put_hevc_bi_pel_pixels64_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, uint8_t *_src, ptrdiff_t _srcstride, int16_t *src2, ptrdiff_t src2stride, int height, intptr_t mx, intptr_t my, int width);

void ff_hevc_put_hevc_bi_pel_pixels16_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, uint8_t *_src, ptrdiff_t _srcstride, int16_t *src2, ptrdiff_t src2stride, int height, intptr_t mx, intptr_t my, int width);
void ff_hevc_put_hevc_bi_pel_pixels24_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, uint8_t *_src, ptrdiff_t _srcstride, int16_t *src2, ptrdiff_t src2stride, int height, intptr_t mx, intptr_t my, int width);
void ff_hevc_put_hevc_bi_pel_pixels32_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, uint8_t *_src, ptrdiff_t _srcstride, int16_t *src2, ptrdiff_t src2stride, int height, intptr_t mx, intptr_t my, int width);
void ff_hevc_put_hevc_bi_pel_pixels48_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, uint8_t *_src, ptrdiff_t _srcstride, int16_t *src2, ptrdiff_t src2stride, int height, intptr_t mx, intptr_t my, int width);
void ff_hevc_put_hevc_bi_pel_pixels64_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, uint8_t *_src, ptrdiff_t _srcstride, int16_t *src2, ptrdiff_t src2stride, int height, intptr_t mx, intptr_t my, int width);
*/
///////////////////////////////////////////////////////////////////////////////
// EPEL
///////////////////////////////////////////////////////////////////////////////
EPEL_PROTOTYPES(epel_h ,  8, sse4_);
EPEL_PROTOTYPES(epel_h , 10, sse4_);
EPEL_PROTOTYPES(epel_h , 12, sse4_);

EPEL_PROTOTYPES(epel_v ,  8, sse4_);
EPEL_PROTOTYPES(epel_v , 10, sse4_);
EPEL_PROTOTYPES(epel_v , 12, sse4_);

EPEL_PROTOTYPES(epel_hv ,  8, sse4_);
EPEL_PROTOTYPES(epel_hv , 10, sse4_);
EPEL_PROTOTYPES(epel_hv , 12, sse4_);

/*//#ifdef OPTI_ASM
PEL_PROTOTYPE(epel_h16, 8, avx2);
PEL_PROTOTYPE(epel_h24, 8, avx2);
PEL_PROTOTYPE(epel_h32, 8, avx2);
PEL_PROTOTYPE(epel_h48, 8, avx2);
PEL_PROTOTYPE(epel_h64, 8, avx2);

PEL_PROTOTYPE(epel_h16,10, avx2);
PEL_PROTOTYPE(epel_h24,10, avx2);
PEL_PROTOTYPE(epel_h32,10, avx2);
PEL_PROTOTYPE(epel_h48,10, avx2);
PEL_PROTOTYPE(epel_h64,10, avx2);

PEL_PROTOTYPE(epel_v16, 8, avx2);
PEL_PROTOTYPE(epel_v24, 8, avx2);
PEL_PROTOTYPE(epel_v32, 8, avx2);
PEL_PROTOTYPE(epel_v48, 8, avx2);
PEL_PROTOTYPE(epel_v64, 8, avx2);

PEL_PROTOTYPE(epel_v16,10, avx2);
PEL_PROTOTYPE(epel_v24,10, avx2);
PEL_PROTOTYPE(epel_v32,10, avx2);
PEL_PROTOTYPE(epel_v48,10, avx2);
PEL_PROTOTYPE(epel_v64,10, avx2);

PEL_PROTOTYPE(epel_hv16, 8, avx2);
PEL_PROTOTYPE(epel_hv24, 8, avx2);
PEL_PROTOTYPE(epel_hv32, 8, avx2);
PEL_PROTOTYPE(epel_hv48, 8, avx2);
PEL_PROTOTYPE(epel_hv64, 8, avx2);

PEL_PROTOTYPE(epel_hv16,10, avx2);
PEL_PROTOTYPE(epel_hv24,10, avx2);
PEL_PROTOTYPE(epel_hv32,10, avx2);
PEL_PROTOTYPE(epel_hv48,10, avx2);
PEL_PROTOTYPE(epel_hv64,10, avx2);
//#endif*/
///////////////////////////////////////////////////////////////////////////////
// QPEL
///////////////////////////////////////////////////////////////////////////////
QPEL_PROTOTYPES(qpel_h ,  8, avx2_);
QPEL_PROTOTYPES(qpel_h , 10, avx2_);
QPEL_PROTOTYPES(qpel_h , 12, avx2_);

QPEL_PROTOTYPES(qpel_v,  8, avx2_);
QPEL_PROTOTYPES(qpel_v, 10, avx2_);
QPEL_PROTOTYPES(qpel_v, 12, avx2_);
QPEL_PROTOTYPES(qpel_v, 16, avx2_);

QPEL_PROTOTYPES(qpel_hv,  8, avx2_);
QPEL_PROTOTYPES(qpel_hv, 10, avx2_);
QPEL_PROTOTYPES(qpel_hv, 12, avx2_);

QPEL_PROTOTYPES(qpel_h ,  8, sse4_);
QPEL_PROTOTYPES(qpel_h , 10, sse4_);
QPEL_PROTOTYPES(qpel_h , 12, sse4_);

QPEL_PROTOTYPES(qpel_v,  8, sse4_);
QPEL_PROTOTYPES(qpel_v, 10, sse4_);
QPEL_PROTOTYPES(qpel_v, 12, sse4_);

QPEL_PROTOTYPES(qpel_hv,  8, sse4_);
QPEL_PROTOTYPES(qpel_hv, 10, sse4_);
QPEL_PROTOTYPES(qpel_hv, 12, sse4_);

PEL_PROTOTYPE(qpel_v16,14,avx2_);
PEL_PROTOTYPE(qpel_v32,14,avx2_);

/*//#ifdef OPTI_ASM
PEL_PROTOTYPE(qpel_h16, 8, avx2);
PEL_PROTOTYPE(qpel_h24, 8, avx2);
PEL_PROTOTYPE(qpel_h32, 8, avx2);
PEL_PROTOTYPE(qpel_h48, 8, avx2);
PEL_PROTOTYPE(qpel_h64, 8, avx2);

PEL_PROTOTYPE(qpel_h16,10, avx2);
PEL_PROTOTYPE(qpel_h24,10, avx2);
PEL_PROTOTYPE(qpel_h32,10, avx2);
PEL_PROTOTYPE(qpel_h48,10, avx2);
PEL_PROTOTYPE(qpel_h64,10, avx2);

PEL_PROTOTYPE(qpel_v16, 8, avx2);
PEL_PROTOTYPE(qpel_v24, 8, avx2);
PEL_PROTOTYPE(qpel_v32, 8, avx2);
PEL_PROTOTYPE(qpel_v48, 8, avx2);
PEL_PROTOTYPE(qpel_v64, 8, avx2);

PEL_PROTOTYPE(qpel_v16,10, avx2);
PEL_PROTOTYPE(qpel_v24,10, avx2);
PEL_PROTOTYPE(qpel_v32,10, avx2);
PEL_PROTOTYPE(qpel_v48,10, avx2);
PEL_PROTOTYPE(qpel_v64,10, avx2);

PEL_PROTOTYPE(qpel_hv16, 8, avx2);
PEL_PROTOTYPE(qpel_hv24, 8, avx2);
PEL_PROTOTYPE(qpel_hv32, 8, avx2);
PEL_PROTOTYPE(qpel_hv48, 8, avx2);
PEL_PROTOTYPE(qpel_hv64, 8, avx2);

PEL_PROTOTYPE(qpel_hv16,10, avx2);
PEL_PROTOTYPE(qpel_hv24,10, avx2);
PEL_PROTOTYPE(qpel_hv32,10, avx2);
PEL_PROTOTYPE(qpel_hv48,10, avx2);
PEL_PROTOTYPE(qpel_hv64,10, avx2);
//#endif*/

WEIGHTING_PROTOTYPES(8, sse4_);
WEIGHTING_PROTOTYPES(10, sse4_);
WEIGHTING_PROTOTYPES(12, sse4_);

WEIGHTING_PROTOTYPES(8, avx2_);
WEIGHTING_PROTOTYPES(10, avx2_);
WEIGHTING_PROTOTYPES(12, avx2_);

///////////////////////////////////////////////////////////////////////////////
// SAO functions
///////////////////////////////////////////////////////////////////////////////
//#ifndef OPTI_ASM
void ff_hevc_sao_edge_filter_8_sse(uint8_t *_dst, uint8_t *_src,
                                  ptrdiff_t stride_dst, ptrdiff_t stride_src,
                                  SAOParams *sao,
                                  int width, int height,
                                  int c_idx);
//#endif
void ff_hevc_sao_edge_filter_10_sse(uint8_t *_dst, uint8_t *_src,
                                  ptrdiff_t stride_dst, ptrdiff_t stride_src,
                                  SAOParams *sao,
                                  int width, int height,
                                  int c_idx);
void ff_hevc_sao_edge_filter_12_sse(uint8_t *_dst, uint8_t *_src,
                                  ptrdiff_t stride_dst, ptrdiff_t stride_src,
                                  SAOParams *sao,
                                  int width, int height,
                                  int c_idx);
//#ifndef OPTI_ASM
void ff_hevc_sao_band_filter_0_8_sse(uint8_t *_dst, uint8_t *_src, ptrdiff_t _stride_dst, ptrdiff_t _stride_src,
                                     struct SAOParams *sao, int *borders, int width, int height, int c_idx);
//#endif
void ff_hevc_sao_band_filter_0_10_sse(uint8_t *_dst, uint8_t *_src, ptrdiff_t _stride_dst, ptrdiff_t _stride_src,
                                      struct SAOParams *sao, int *borders, int width, int height, int c_idx);
void ff_hevc_sao_band_filter_0_12_sse(uint8_t *_dst, uint8_t *_src, ptrdiff_t _stride_dst, ptrdiff_t _stride_src,
                                      struct SAOParams *sao, int *borders, int width, int height, int c_idx);

//#ifdef SVC_EXTENSION

    void ff_upsample_filter_block_luma_h_all_sse(int16_t *dst, ptrdiff_t dststride, uint8_t *_src, ptrdiff_t _srcstride,
            int x_EL, int x_BL, int block_w, int block_h, int widthEL,
            const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);
    void ff_upsample_filter_block_cr_h_all_sse(int16_t *dst, ptrdiff_t dststride, uint8_t *_src, ptrdiff_t _srcstride,
                int x_EL, int x_BL, int block_w, int block_h, int widthEL,
                const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);
    void ff_upsample_filter_block_luma_v_all_sse(uint8_t *dst, ptrdiff_t dststride, int16_t *_src, ptrdiff_t _srcstride,
            int y_BL, int x_EL, int y_EL, int block_w, int block_h, int widthEL, int heightEL,
            const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);
    void ff_upsample_filter_block_cr_v_all_sse(uint8_t *dst, ptrdiff_t dststride, int16_t *_src, ptrdiff_t _srcstride,
            int y_BL, int x_EL, int y_EL, int block_w, int block_h, int widthEL, int heightEL,
            const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);

    void ff_upsample_filter_block_luma_h_x2_sse(int16_t *dst, ptrdiff_t dststride, uint8_t *_src, ptrdiff_t _srcstride,
            int x_EL, int x_BL, int block_w, int block_h, int widthEL,
            const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);
    void ff_upsample_filter_block_cr_h_x2_sse(int16_t *dst, ptrdiff_t dststride, uint8_t *_src, ptrdiff_t _srcstride,
                int x_EL, int x_BL, int block_w, int block_h, int widthEL,
                const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);
    void ff_upsample_filter_block_luma_v_x2_sse(uint8_t *dst, ptrdiff_t dststride, int16_t *_src, ptrdiff_t _srcstride,
            int y_BL, int x_EL, int y_EL, int block_w, int block_h, int widthEL, int heightEL,
            const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);
    void ff_upsample_filter_block_cr_v_x2_sse(uint8_t *dst, ptrdiff_t dststride, int16_t *_src, ptrdiff_t _srcstride,
            int y_BL, int x_EL, int y_EL, int block_w, int block_h, int widthEL, int heightEL,
            const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);

    void ff_upsample_filter_block_luma_h_x1_5_sse(int16_t *dst, ptrdiff_t dststride, uint8_t *_src, ptrdiff_t _srcstride,
            int x_EL, int x_BL, int block_w, int block_h, int widthEL,
            const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);
    void ff_upsample_filter_block_cr_h_x1_5_sse(int16_t *dst, ptrdiff_t dststride, uint8_t *_src, ptrdiff_t _srcstride,
                int x_EL, int x_BL, int block_w, int block_h, int widthEL,
                const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);
    void ff_upsample_filter_block_luma_v_x1_5_sse(uint8_t *dst, ptrdiff_t dststride, int16_t *_src, ptrdiff_t _srcstride,
            int y_BL, int x_EL, int y_EL, int block_w, int block_h, int widthEL, int heightEL,
            const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);
    void ff_upsample_filter_block_cr_v_x1_5_sse(uint8_t *dst, ptrdiff_t dststride, int16_t *_src, ptrdiff_t _srcstride,
            int y_BL, int x_EL, int y_EL, int block_w, int block_h, int widthEL, int heightEL,
            const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);

    void ff_upsample_filter_block_luma_h_8_8_sse( int16_t *dst, ptrdiff_t dststride, uint8_t *_src, ptrdiff_t _srcstride,
            int x_EL, int x_BL, int block_w, int block_h, int widthEL,
            const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);
    void ff_upsample_filter_block_luma_v_8_8_sse(uint8_t *dst, ptrdiff_t dststride, int16_t *_src, ptrdiff_t _srcstride,
            int y_BL, int x_EL, int y_EL, int block_w, int block_h, int widthEL, int heightEL,
            const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);
   void ff_upsample_filter_block_cr_h_8_8_sse( int16_t *dst, ptrdiff_t dststride, uint8_t *_src, ptrdiff_t _srcstride,
            int x_EL, int x_BL, int block_w, int block_h, int widthEL,
            const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);
   void ff_upsample_filter_block_cr_v_8_8_sse(uint8_t *dst, ptrdiff_t dststride, int16_t *_src, ptrdiff_t _srcstride,
           int y_BL, int x_EL, int y_EL, int block_w, int block_h, int widthEL, int heightEL,
           const struct HEVCWindow *Enhscal, struct UpsamplInf *up_info);
//#endif

#endif // AVCODEC_X86_HEVCDSP_H
