/*
 *    mantissa.c
 *
 *	Copyright (C) Aaron Holtzman - May 1999
 *
 *  This file is part of ac3dec, a free Dolby AC-3 stream decoder.
 *
 *  ac3dec is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  ac3dec is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  AC3DEC_WAVE
 *  -----------
 *   v0.05 made it hell faster thanks to better management of pointers (CHEEKYBOY)
 *   v0.04 modified mantissa.c to return #errors


calling layout:
bitstream_get
bitstream_load
my_fread
pull_data
fill_queue
my_autofillHandlerDemuxAC3
demux_ac3
demux_getbits
demux_refill_buffer
FastReadChar (fread)
 */


#include <stdlib.h>
#include <stdio.h>
#include "ac3.h"
#include "decode.h"
#include "bitstream.h"
#include "mantissa.h"

//Lookup tables of 0.16 two's complement quantization values
const uint_16 q_1[3] = {( -2 << 15)/3, 0           ,(  2 << 15)/3 };

const uint_16 q_2[5] = {( -4 << 15)/5,( -2 << 15)/5,   0         ,
	                (  2 << 15)/5,(  4 << 15)/5};

const uint_16 q_3[7] = {( -6 << 15)/7,( -4 << 15)/7,( -2 << 15)/7,
	                   0         ,(  2 << 15)/7,(  4 << 15)/7,
									(  6 << 15)/7};

const uint_16 q_4[11] = {(-10 << 15)/11,(-8 << 15)/11,(-6 << 15)/11,
	                ( -4 << 15)/11,(-2 << 15)/11,  0          ,
									(  2 << 15)/11,( 4 << 15)/11,( 6 << 15)/11,
									(  8 << 15)/11,(10 << 15)/11};

const uint_16 q_5[15] = {(-14 << 15)/15,(-12 << 15)/15,(-10 << 15)/15,
                   ( -8 << 15)/15,( -6 << 15)/15,( -4 << 15)/15,
									 ( -2 << 15)/15,   0          ,(  2 << 15)/15,
									 (  4 << 15)/15,(  6 << 15)/15,(  8 << 15)/15,
									 ( 10 << 15)/15,( 12 << 15)/15,( 14 << 15)/15};

//These store the persistent state of the packed mantissas
static uint_16 m_1[3];
static uint_16 m_2[3];
static uint_16 m_4[2];
static uint_16 m_1_pointer;
static uint_16 m_2_pointer;
static uint_16 m_4_pointer;

//uint_16 . fast manitsa lookups instead of calculating it
static uint_16	fastM = 0;
static uint_16	fastM_9[32][3];
static uint_16	fastM_25[128][3];
static uint_16	fastM_11[128][3];


//Conversion from bap to number of bits in the mantissas
//zeros account for cases 0,1,2,4 which are special cased
static uint_16 qnttztab[16] = { 0, 0, 0, 3, 0 , 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16};

static uint_16 mantissa_get(bitstream_t *bs, uint_16 bap );
static void mantissa_reset(void);

static uint_32 cbErrors = 0; /* error counter */



static int	do_dither;
uint_32
mantissa_unpack(bsi_t *bsi, audblk_t *audblk,bitstream_t *bs)
{
	uint_16 i,j;
	uint_32 start;
	uint_32 done_cpl = 0;
	register uint_16 *mant_ptr, *bap_ptr;

	mantissa_reset();

	if ( !fastM ){
		makeFastMantissaLookups();
		fastM = 1;
	}


	//FIXME remove
	start = bs->total_bits_read;

	for(i=0; i< bsi->nfchans; i++){
		mant_ptr = &audblk->chmant[i][0];
		bap_ptr = &audblk->fbw_bap[i][0];
		do_dither = audblk->dithflag[i];

		for(j=0; j < audblk->endmant[i]; j++){
			*mant_ptr++ = mantissa_get( bs, *bap_ptr++ );
		}

		if(audblk->cplinu && audblk->chincpl[i] && !(done_cpl))
		{
			j=audblk->cplstrtmant;
			mant_ptr = &audblk->cplmant[j];
			bap_ptr = &audblk->cpl_bap[j];

			/* ncplmant is equal to 12 * ncplsubnd */
			for(j=audblk->cplstrtmant; j < audblk->cplendmant; j++){
				*mant_ptr++ = mantissa_get( bs, *bap_ptr++ );
			}

			done_cpl = 1;
		}
	}

	if(bsi->lfeon){
		mant_ptr = &audblk->lfemant[0];
		bap_ptr = &audblk->lfe_bap[0];

		/* There are always 7 mantissas for lfe */

		*mant_ptr++ = mantissa_get(bs, *bap_ptr++ );
		*mant_ptr++ = mantissa_get(bs, *bap_ptr++ );
		*mant_ptr++ = mantissa_get(bs, *bap_ptr++ );
		*mant_ptr++ = mantissa_get(bs, *bap_ptr++ );
		*mant_ptr++ = mantissa_get(bs, *bap_ptr++ );
		*mant_ptr++ = mantissa_get(bs, *bap_ptr++ );
		*mant_ptr++ = mantissa_get(bs, *bap_ptr++ );
		
/*		for(j=0; j < 7 ; j++){
			//audblk->lfemant[j] = mantissa_get(bs,audblk->lfe_bap[j], &fError );
			*mant_ptr++ = mantissa_get(bs, *bap_ptr++ );
		}
*/
	}

	return cbErrors; /* return # errors */
}



// do fast lookups instead of shit slow Divs/Mods
// should be hell fast now!!!
// if i could i would use 16megs of ram for everysingle possible math table
static	makeFastMantissaLookups( void )
{
	long i;

	for( i=0; i<1<<5; i++){
		fastM_9[i][0] = i / 9;
		fastM_9[i][1] = (i % 9) / 3;
		fastM_9[i][2] = (i % 9) % 3;
	}
	for( i=0; i<1<<7; i++){
		fastM_25[i][0] = i / 25;
		fastM_25[i][1] = (i % 25) / 5;
		fastM_25[i][2] = (i % 25) % 5;
	}
	for( i=0; i<1<<7; i++){
		fastM_11[i][0] = i / 11;
		fastM_11[i][1] = i % 11;
	}
}




#define	mantissa_get_dither	0		//	return rand();


/* Fetch an unpacked, left justified, and properly biased/dithered mantissa value */
static uint_16
mantissa_get(bitstream_t *bs, uint_16 bap )
{
	uint_16 result, index;
	uint_16 group_code, *fast_m;
static uint_16 *fast_m_1, *fast_m_2, *fast_m_4;

	//If the bap is 0-5 then we have special cases to take care of
	switch(bap)
	{
		case 0:
			//FIXME change to respect the dither flag
			if ( do_dither ){
				result = (rand()*(32767*2)) - 32767;
				result = result / 2;
			} else
				result = 0;
			break;

		case 1:
			if(m_1_pointer > 2)
			{
				group_code = bitstream_get(bs,5);

				fast_m_1 = &fastM_9[group_code][0];
/*				m_1[0] = *fast_m++;//group_code / 9;
				m_1[1] = *fast_m++;//(group_code % 9) / 3;
				m_1[2] = *fast_m++;//(group_code % 9) % 3;
*/
				m_1_pointer = 0;
			}
			index = fast_m_1[m_1_pointer++];
			result = q_1[index];
			break;
		case 2:
			if(m_2_pointer > 2)
			{
				group_code = bitstream_get(bs,7);

				fast_m_2 = &fastM_25[group_code][0];
/*				m_2[0] = *fast_m++;//group_code / 25;
				m_2[1] = *fast_m++;//(group_code % 25) / 5 ;
				m_2[2] = *fast_m++;//(group_code % 25) % 5 ;
*/				m_2_pointer = 0;
			}
			index = fast_m_2[m_2_pointer++];
			result = q_2[index];
			break;

		case 3:
			result = bitstream_get(bs,3);

			result = q_3[result];
			break;

		case 4:
			if(m_4_pointer > 1)
			{
				group_code = bitstream_get(bs,7);

				fast_m_4 = &fastM_11[group_code][0];
/*				m_4[0] = *fast_m++;//group_code / 11;
				m_4[1] = *fast_m++;//group_code % 11;
*/				m_4_pointer = 0;
			}
			index = fast_m_4[m_4_pointer++];
			result = q_4[index];
			break;

		case 5:
			result = bitstream_get(bs,4);

			result = q_5[result];
			break;

		default:
			result = bitstream_get(bs,qnttztab[bap]);
			result <<= 16 - qnttztab[bap];
	}

	return result;
}

static void
mantissa_reset(void)
{
	m_1[2] = m_1[1] = m_1[0] = 0;
	m_2[2] = m_2[1] = m_2[0] = 0;
	m_4[1] = m_4[0] = 0;
	m_1_pointer = m_2_pointer = m_4_pointer = 3;
}

