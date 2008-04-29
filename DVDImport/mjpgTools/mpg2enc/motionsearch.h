// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// motionsearch.h, part of the mjpgTools Library																		 //
// --------------------------------------------------------------------------------------------------------------------- //
// Copyleft(c)2002 by Laurens KOEHOORN <https://sourceforge.net/sendmessage.php?touser=240832>							 //
//																														 //
// This library is free software; you can redistribute it and/or modify it under the terms of the						 //
// GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License,	 //
// or (at your option) any later version.																				 //
//																														 //
// This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied	 //
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.														 //
// See the GNU Lesser General Public License for more details.															 //
//																														 //
// You should have received a copy of the GNU Lesser General Public License along with this library; if not,			 //
// write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA					 //
// --------------------------------------------------------------------------------------------------------------------- //
// This library is globaly based on :																					 //
//  - Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved.											 //
//																														 //
//		Disclaimer of Warranty																							 //
//		These software programs are available to the user without any license fee or royalty on an "as is" basis.		 //
//		The MPEG Software Simulation Group disclaims any and all warranties, whether express, implied, or statuary,		 //
//		including any implied warranties or merchantability or of fitness for a particular purpose.						 //
//		In no event shall the copyright-holder be liable for any incidental, punitive, or consequential damages of any	 //
//		kind whatsoever arising from the use of these programs.															 //
//		This disclaimer of warranty extends to the user of these programs and user's customers, employees, agents,		 //
//		transferees, successors, and assigns.																			 //
//		The MPEG Software Simulation Group does not represent or warrant that the programs furnished hereunder are free	 //
//		of infringement of any third-party patents.																		 //
//		Commercial implementations of MPEG-1 and MPEG-2 video, including shareware, are subject to royalty fees to		 //
//		patent holders. Many of these patents are general enough such that they are unavoidable regardless of			 //
//		implementation design.																							 //
//																														 //
// and partialy based on the modifications made by :																	 //
//	- Modifications and enhancements (C) 2000/2001 Andrew Stevens (MJPeGtools of the Linux-group)						 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
#ifndef __MOTIONSEARCH_H__
#define __MOTIONSEARCH_H__

#include "types.h"
//#include <config.h>
//#include "mjpeg_types.h"

struct me_result
{
	uint16_t weight;
	int8_t x;
	int8_t y;
};

typedef struct me_result me_result_s;

#define MAX_MATCHES (256*256/4)

typedef struct _me_result_vec {
	int len;
	me_result_s mests[MAX_MATCHES];
} me_result_set;

/*
 * Function pointers for selecting CPU specific implementations
 *
 * Top-level motion estimation entry points.
 *
 */

/*
 * Use given set of 2*2 sub-sampled motion estimates generated using
 * pbuild_sub22_mests to generate a best 1-pel motion estimate.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

extern void (*pfind_best_one_pel)( me_result_set *sub22set,
								   uint8_t *org, uint8_t *blk,
								   int i0, int j0,
								   int ihigh, int jhigh,
								   int rowstride, int h, 
								   me_result_s *res
	);

/*
 * Use given set of 4*4 sub-sampled motion estimates generated using
 * pbuild_sub44_mests to generate a set of best 2*2 sub-sampled motion
 * estimates.
 */

extern int (*pbuild_sub22_mests)( me_result_set *sub44set,
								  me_result_set *sub22set,
								  int i0,  int j0, int ihigh, int jhigh, 
								  int null_mc_sad,
								  uint8_t *s22org,  uint8_t *s22blk, 
								  int frowstride, int fh,
								  int reduction
	);

/*
 * Generate a set the best 4*4 sub-sampled motion estimates
 * from the specifed rectangle of candidates in s44org with respect
 * to the reference block s44blk
 */

extern int (*pbuild_sub44_mests)( me_result_set *sub44set,
								  int ilow, int jlow, int ihigh, int jhigh, 
								  int i0, int j0,
								  int null_mc_sad,
								  uint8_t *s44org, uint8_t *s44blk, 
								  int qrowstride, int qh,
								  int reduction );


/*
 * Lower level difference comparison routines
 *
 * Naming convention: for block difference metric functions
 * sad - Sum Absolute difference
 * sumsq - Sum of squares
 * sub44 - 4*4 Subsampled macroblocks (i.e. 4x4 subsampled pels)
 * sub22 2*2 submsapled macroblocks (i.e. 8x8 subsampled pels)
 * <nothing> Ordinary macroblocks (i.e. 16x16 pels)
 * "b" prefix:  Difference of bi-directionally interpolated block 
 *              (i.e. mean of two blocks)
 * _xy suffix:  Difference with half-pel sub-sampling offset.
 *              (i.e. _00 is just ordinary no sub-sampling)
 * mests - Motion estimate results relative motion vector with sad.
 */

extern int (*psumsq_sub22)( uint8_t *blk1, uint8_t *blk2,
						 int rowstride, int h);
extern int (*pbsumsq_sub22)( uint8_t *blk1f, uint8_t *blk1b, 
						  uint8_t *blk2,
						  int rowstride, int h);

extern int (*pvariance)(uint8_t *mb, int size, int rowstride);


extern int (*psad_sub22) ( uint8_t *blk1, uint8_t *blk2,  int frowstride, int fh);
extern int (*psad_sub44) ( uint8_t *blk1, uint8_t *blk2,  int qrowstride, int qh);
extern int (*psad_00) ( uint8_t *blk1, uint8_t *blk2,  int rowstride, int h, int distlim);
extern int (*psad_01) (uint8_t *blk1, uint8_t *blk2, int rowstride, int h);
extern int (*psad_10) (uint8_t *blk1, uint8_t *blk2, int rowstride, int h);
extern int (*psad_11) (uint8_t *blk1, uint8_t *blk2, int rowstride, int h);

extern int (*psumsq) (uint8_t *blk1, uint8_t *blk2,
					  int rowstride, int hx, int hy, int h);
  
  
extern int (*pbsumsq) (uint8_t *pf, uint8_t *pb,
					   uint8_t *p2, int rowstride, int hxf, int hyf, int hxb, int hyb, int h);

extern int (*pbsad) (uint8_t *pf, uint8_t *pb,
					 uint8_t *p2, int rowstride, int hxf, int hyf, int hxb, int hyb, int h);

void init_motion_search(void);
int round_search_radius( int radius );
void subsample_image( uint8_t *image, int rowstride, 
					  uint8_t *sub22_image, 
					  uint8_t *sub44_image);


#ifdef __cplusplus
}
#endif

#endif  /* __MOTIONSEARCH_H__ */