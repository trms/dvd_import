/* 
 *    parse.c
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
 *   v0.05 - minor tweak to audblk->rematstr parsing, non-used flags are
 *           now cleared (set to 0),
 *           correction for document section 5.4.3.47 (from errata sheet)
 *   v0.04 - modified parse_audblk() to prevent data contamination from
 *           previously decoded audioblocks, minor bugfixes
 *           moved look up tables to parse.h
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "ac3.h"
#include "decode.h"
#include "bitstream.h"
#include "stats.h"
#include "debug.h"


#define _PARSE_C 1
#include "parse.h"
/* _PARSE_C tells parse.h to declare the tables */

/* v0.04 Misc LUT moved to parse.h */

/* Parse a syncinfo structure, minus the sync word */
void
parse_syncinfo(syncinfo_t *syncinfo,bitstream_t *bs)
{

	/* Get crc1 - we don't actually use this data though */
	bitstream_get(bs,16);

	/* Get the sampling rate */
	syncinfo->fscod  = bitstream_get(bs,2);

	/* Get the frame size code */
	syncinfo->frmsizecod = bitstream_get(bs,6);

	syncinfo->bit_rate = frmsizecod_tbl[syncinfo->frmsizecod].bit_rate;
	syncinfo->frame_size = frmsizecod_tbl[syncinfo->frmsizecod].frm_size[syncinfo->fscod];

	stats_printf_syncinfo(syncinfo);
}

/*
 * This routine fills a bsi struct from the AC3 stream
 */

void
parse_bsi(bsi_t *bsi,bitstream_t *bs)
{
	uint_32 i;

	/* Check the AC-3 version number */
	bsi->bsid = bitstream_get(bs,5);

	/* Get the audio service provided by the steram */
	bsi->bsmod = bitstream_get(bs,3);

	/* Get the audio coding mode (ie how many channels)*/
	bsi->acmod = bitstream_get(bs,3);
	/* Predecode the number of full bandwidth channels as we use this
	 * number a lot */
	bsi->nfchans = nfchans[bsi->acmod];

	/* If it is in use, get the centre channel mix level */
	if ((bsi->acmod & 0x1) && (bsi->acmod != 0x1))
		bsi->cmixlev = bitstream_get(bs,2);

	/* If it is in use, get the surround channel mix level */
	if (bsi->acmod & 0x4)
		bsi->surmixlev = bitstream_get(bs,2);

	/* Get the dolby surround mode if in 2/0 mode */
	if(bsi->acmod == 0x2)
		bsi->dsurmod= bitstream_get(bs,2);

	/* Is the low frequency effects channel on? */
	bsi->lfeon = bitstream_get(bs,1);

	/* Get the dialogue normalization level */
	bsi->dialnorm = bitstream_get(bs,5);

	/* Does compression gain exist? */
	bsi->compre = bitstream_get(bs,1);
	if (bsi->compre)
	{
		/* Get compression gain */
		bsi->compr = bitstream_get(bs,8);
	}

	/* Does language code exist? */
	bsi->langcode = bitstream_get(bs,1);
	if (bsi->langcode)
	{
		/* Get langauge code */
		bsi->langcod = bitstream_get(bs,8);
	}

	/* Does audio production info exist? */
	bsi->audprodie = bitstream_get(bs,1);
	if (bsi->audprodie)
	{
		/* Get mix level */
		bsi->mixlevel = bitstream_get(bs,5);

		/* Get room type */
		bsi->roomtyp = bitstream_get(bs,2);
	}

	/* If we're in dual mono mode then get some extra info */
	if (bsi->acmod ==0)
	{
		/* Get the dialogue normalization level two */
		bsi->dialnorm2 = bitstream_get(bs,5);

		/* Does compression gain two exist? */
		bsi->compr2e = bitstream_get(bs,1);
		if (bsi->compr2e)
		{
			/* Get compression gain two */
			bsi->compr2 = bitstream_get(bs,8);
		}

		/* Does language code two exist? */
		bsi->langcod2e = bitstream_get(bs,1);
		if (bsi->langcod2e)
		{
			/* Get langauge code two */
			bsi->langcod2 = bitstream_get(bs,8);
		}

		/* Does audio production info two exist? */
		bsi->audprodi2e = bitstream_get(bs,1);
		if (bsi->audprodi2e)
		{
			/* Get mix level two */
			bsi->mixlevel2 = bitstream_get(bs,5);

			/* Get room type two */
			bsi->roomtyp2 = bitstream_get(bs,2);
		}
	}

	/* Get the copyright bit */
	bsi->copyrightb = bitstream_get(bs,1);

	/* Get the original bit */
	bsi->origbs = bitstream_get(bs,1);
	
	/* Does timecode one exist? */
	bsi->timecod1e = bitstream_get(bs,1);

	if(bsi->timecod1e)
		bsi->timecod1 = bitstream_get(bs,14);

	/* Does timecode two exist? */
	bsi->timecod2e = bitstream_get(bs,1);

	if(bsi->timecod2e)
		bsi->timecod2 = bitstream_get(bs,14);

	/* Does addition info exist? */
	bsi->addbsie = bitstream_get(bs,1);

	if(bsi->addbsie)
	{
		/* Get how much info is there */
		bsi->addbsil = bitstream_get(bs,6);

		/* Get the additional info */
		for(i=0;i<(bsi->addbsil + 1);i++)
			bsi->addbsi[i] = bitstream_get(bs,8);
	}

	stats_printf_bsi(bsi);
}

/* More pain inducing parsing */
void 
parse_audblk(bsi_t *bsi,audblk_t *audblk,bitstream_t *bs, uint_16 nblk)
{
	int i,j;
	uint_16 temp_uint16; /* temporary scratch variable */

	/* v0.04 - start with zero'd structure, to prevent "contamination"
	   from previously decoded  audioblock*/

	if ( nblk == 0 ) /* audioblock 0?  If so, reset these parameters */
	{
		/* 5.4.3.4 - 5.4.3.6 ( dynrng,dynrng2 reset to 0 in audioblock#0) */
		audblk->dynrng = 0;
		audblk->dynrng2 = 0;
	}

	for (i=0;i < bsi->nfchans; i++)
	{
		/* Is this channel an interleaved 256 + 256 block ? */
		audblk->blksw[i] = bitstream_get(bs,1);
	}

	for (i=0;i < bsi->nfchans; i++)
	{
		/* Should we dither this channel? */
		audblk->dithflag[i] = bitstream_get(bs,1);
	}

	/* Does dynamic range control exist? */
	audblk->dynrnge = bitstream_get(bs,1);
	if (audblk->dynrnge)
	{
		/* Get dynamic range info */
		audblk->dynrng = bitstream_get(bs,8);
	}

	/* If we're in dual mono mode then get the second channel DR info */
	if (bsi->acmod == 0)
	{
		/* Does dynamic range control two exist? */
		audblk->dynrng2e = bitstream_get(bs,1);
		if (audblk->dynrng2e)
		{
			/* Get dynamic range info */
			audblk->dynrng2 = bitstream_get(bs,8);
		}
	}

	/* Does coupling strategy exist? */

	audblk->cplstre = bitstream_get(bs,1);
	if (audblk->cplstre)
	{
		/* Is coupling turned on? */
		audblk->cplinu = bitstream_get(bs,1);
		if(audblk->cplinu)
		{
			for(i=0;i < bsi->nfchans; i++)
				audblk->chincpl[i] = bitstream_get(bs,1);
			if(bsi->acmod == 0x2)
				audblk->phsflginu = bitstream_get(bs,1);
			audblk->cplbegf = bitstream_get(bs,4);
			audblk->cplendf = bitstream_get(bs,4);
			if ( audblk->cplbegf > audblk->cplendf )
			{
				fprintf( stderr, " hmm, cplendf=%u < cplbegf=%u\n"
					, audblk->cplendf,audblk->cplbegf);
				temp_uint16 = audblk->cplbegf;
				audblk->cplbegf = audblk->cplendf;
				audblk->cplendf = temp_uint16;
			}

			audblk->ncplsubnd = (audblk->cplendf + 2) - audblk->cplbegf + 1; 

			/* Calculate the start and end bins of the coupling channel */
			audblk->cplstrtmant = (audblk->cplbegf * 12) + 37 ; 
			audblk->cplendmant =  ((audblk->cplendf + 3) * 12) + 37;

			/* The number of combined subbands is ncplsubnd minus each combined
			 * band */
			audblk->ncplbnd = audblk->ncplsubnd; 

			/* v0.04 rsl, first element of array always = 0 */
			audblk->cplbndstrc[0] = 0;
			for(i=1; i< audblk->ncplsubnd; i++)
			{
				audblk->cplbndstrc[i] = bitstream_get(bs,1);
				audblk->ncplbnd -= audblk->cplbndstrc[i];
			}

		}
	}

				if ( audblk->ncplbnd > 1000 )
				{
					printf( " opps ncplbnd = %u, ncplsubnd = %u\n",
						audblk->ncplbnd, audblk->ncplsubnd );
					exit(1);
				}


	if( audblk->cplinu )
	{
		/* Loop through all the channels and get their coupling co-ords */	
		for(i=0;i < bsi->nfchans;i++)
		{
			if(!audblk->chincpl[i])
				continue;

			/* Is there new coupling co-ordinate info? */
			audblk->cplcoe[i] = bitstream_get(bs,1);

			if(audblk->cplcoe[i])
			{
				audblk->mstrcplco[i] = bitstream_get(bs,2); 
				for(j=0;j < audblk->ncplbnd; j++)
				{
					audblk->cplcoexp[i][j] = bitstream_get(bs,4); 
					audblk->cplcomant[i][j] = bitstream_get(bs,4); 
				}
			}
		}

		/* If we're in 2/0 (stereo) mode, there's going to be some phase info */
		if( (bsi->acmod == 0x2) && audblk->phsflginu && 
				(audblk->cplcoe[0] || audblk->cplcoe[1]))
		{
			for(j=0;j < audblk->ncplbnd; j++)
				audblk->phsflg[j] = bitstream_get(bs,1); 

		}
	}

	/* If we're in 2/0 (stereo) mode, there may be a rematrix strategy */
	if(bsi->acmod == 0x2)
	{
		audblk->rematstr = bitstream_get(bs,1);
		if(audblk->rematstr)
		{
			if (audblk->cplinu == 0) 
			{ 
				for(i = 0; i < 4; i++) 
					audblk->rematflg[i] = bitstream_get(bs,1);
			}
			if((audblk->cplbegf > 2) && audblk->cplinu) 
			{
				for(i = 0; i < 4; i++) 
					audblk->rematflg[i] = bitstream_get(bs,1);
			}
			if((audblk->cplbegf <= 2) && audblk->cplinu) 
			{ 
				for(i = 0; i < 3; i++) 
					audblk->rematflg[i] = bitstream_get(bs,1);
				audblk->rematflg[3] = 0;
			} 
			if((audblk->cplbegf == 0) && audblk->cplinu) 
			{
				for(i = 0; i < 2; i++) 
					audblk->rematflg[i] = bitstream_get(bs,1);
				audblk->rematflg[2] = 0;
				audblk->rematflg[3] = 0;
			}

		}
	}

	if (audblk->cplinu )
	//if (audblk->cplinu && audblk->cplexpstr != EXP_REUSE )
	{
		/* Get the coupling channel exponent strategy */
		audblk->cplexpstr = bitstream_get(bs,2);
		/* moved ncplgrps calculation to later 
		audblk->ncplgrps = (audblk->cplendmant - audblk->cplstrtmant) / 
				(3 << ( cplexpstr-1)); */
	}

	for(i = 0; i < bsi->nfchans; i++)
		audblk->chexpstr[i] = bitstream_get(bs,2);

	/* Get the exponent strategy for lfe channel */
	if(bsi->lfeon) 
		audblk->lfeexpstr = bitstream_get(bs,1);

	/* Determine the bandwidths of all the fbw channels */
	for(i = 0; i < bsi->nfchans; i++) 
	{ 
		uint_16 grp_size;

		if(audblk->chexpstr[i] != EXP_REUSE) 
		{ 
// rsl didn't work		if ( audblk->chincpl[i] ) 
			if (audblk->cplinu && audblk->chincpl[i]) 
			{
				audblk->endmant[i] = audblk->cplstrtmant;
			}
			else
			{
				audblk->chbwcod[i] = bitstream_get(bs,6); 
				audblk->endmant[i] = ((audblk->chbwcod[i] + 12) * 3) + 37;
			}

			/* Calculate the number of exponent groups to fetch */
			grp_size =  3 * (1 << (audblk->chexpstr[i] - 1));
			audblk->nchgrps[i] = (audblk->endmant[i] - 1 + (grp_size - 3)) / grp_size;
		}
	}

	/* Get the coupling exponents if they exist */
	if(audblk->cplinu && (audblk->cplexpstr != EXP_REUSE))
	{
		audblk->cplabsexp = bitstream_get(bs,4);

		/* rsl test, calculate new ncplgrps */
		switch ( (audblk->cplexpstr) )
		{
			case 1 :
				audblk->ncplgrps = (1+audblk->cplendmant - audblk->cplstrtmant)
					/ 3;
				break;
			case 2 :
				audblk->ncplgrps = (3+audblk->cplendmant - audblk->cplstrtmant)
					/ 6;
				break;
			case 3 :
				audblk->ncplgrps = (6+audblk->cplendmant - audblk->cplstrtmant)
					/ 12;
				break;
			default:
				fprintf( stderr, "\nparse() :switch() {} opps\n");
				exit(1);
		}

		for(i=0;i< audblk->ncplgrps;i++)
			audblk->cplexps[i] = bitstream_get(bs,7);
	}

	/* Get the fwb channel exponents */
	for(i=0;i < bsi->nfchans; i++)
	{
		if(audblk->chexpstr[i] != EXP_REUSE)
		{
			audblk->exps[i][0] = bitstream_get(bs,4);			
			for(j=1;j<=audblk->nchgrps[i];j++)
				audblk->exps[i][j] = bitstream_get(bs,7);
			audblk->gainrng[i] = bitstream_get(bs,2);
		}
	}

	/* Get the lfe channel exponents */
	if(bsi->lfeon && (audblk->lfeexpstr != EXP_REUSE))
	{
		audblk->lfeexps[0] = bitstream_get(bs,4);
		audblk->lfeexps[1] = bitstream_get(bs,7);
		audblk->lfeexps[2] = bitstream_get(bs,7);
	}

	/* Get the parametric bit allocation parameters */
	audblk->baie = bitstream_get(bs,1);

	if(audblk->baie)
	{
		audblk->sdcycod = bitstream_get(bs,2);
		audblk->fdcycod = bitstream_get(bs,2);
		audblk->sgaincod = bitstream_get(bs,2);
		audblk->dbpbcod = bitstream_get(bs,2);
		audblk->floorcod = bitstream_get(bs,3);
	}

	/* Get the SNR off set info if it exists */
	audblk->snroffste = bitstream_get(bs,1);

	if(audblk->snroffste)
	{
		audblk->csnroffst = bitstream_get(bs,6);

		if(audblk->cplinu)
		{
			audblk->cplfsnroffst = bitstream_get(bs,4);
			audblk->cplfgaincod = bitstream_get(bs,3);
		}

		for(i = 0;i < bsi->nfchans; i++)
		{
			audblk->fsnroffst[i] = bitstream_get(bs,4);
			audblk->fgaincod[i] = bitstream_get(bs,3);
		}
		if(bsi->lfeon)
		{

			audblk->lfefsnroffst = bitstream_get(bs,4);
			audblk->lfefgaincod = bitstream_get(bs,3);
		}
	}

	/* Get coupling leakage info if it exists */
	if(audblk->cplinu)
	{
		audblk->cplleake = bitstream_get(bs,1);	
		
		if(audblk->cplleake)
		{
			audblk->cplfleak = bitstream_get(bs,3);
			audblk->cplsleak = bitstream_get(bs,3);
		}
	}
	
	/* Get the delta bit alloaction info */
	audblk->deltbaie = bitstream_get(bs,1);	
	
	/* v0.04 */
	if ( ( !audblk->deltbaie ) && ( nblk == 0 ) )
	{	/*5.4.3.47, reset cpldeltnseg and deltnseg[ch] */
		/* errata */
		audblk->cpldeltbae = 2; /* binary '10', no delta alloc */
		//audblk->cpldeltnseg = 0;
		for( i = 0; i < bsi->nfchans; ++i )
			audblk->deltbae[ i ] = 2; /* binary '10', no delta alloc */
			//audblk->deltnseg[ i ] = 0;
	}

	if(audblk->deltbaie)
	{
		if(audblk->cplinu)
			audblk->cpldeltbae = bitstream_get(bs,2);

		for(i = 0;i < bsi->nfchans; i++)
			audblk->deltbae[i] = bitstream_get(bs,2);

		if (audblk->cplinu && (audblk->cpldeltbae == DELTA_BIT_NEW))
		{
			audblk->cpldeltnseg = bitstream_get(bs,3);
			for(i = 0;i < audblk->cpldeltnseg + 1; i++)
			{
				audblk->cpldeltoffst[i] = bitstream_get(bs,5);
				audblk->cpldeltlen[i] = bitstream_get(bs,4);
				audblk->cpldeltba[i] = bitstream_get(bs,3);
			}
		}

		for(i = 0;i < bsi->nfchans; i++)
		{
			if (audblk->cplinu && (audblk->deltbae[i] == DELTA_BIT_NEW))
			{
				audblk->deltnseg[i] = bitstream_get(bs,3);
				/* v0.04 for(j = 0; j < audblk->cpldeltnseg + 1; j++) */
				for(j = 0; j < audblk->deltnseg[i] + 1; j++)
				{
					audblk->deltoffst[i][j] = bitstream_get(bs,5);
					audblk->deltlen[i][j] = bitstream_get(bs,4);
					audblk->deltba[i][j] = bitstream_get(bs,3);
				}
			}
		}
	}

	/* Check to see if there's any dummy info to get */
	audblk->skiple =  bitstream_get(bs,1);
	if( audblk->skiple )
	{
		uint_16 skip_data;

		audblk->skipl = bitstream_get(bs,9);
		//FIXME remove
		//fprintf(stderr," (parse) skipping %d bytes\n",audblk->skipl);

		for(i = 0; i < audblk->skipl ; i++)
		{
			skip_data = bitstream_get(bs,8);
			//FIXME remove
			//fprintf(stderr,"skipped data %2x\n",skip_data);
			//if(skip_data != 0)
			//{	
				//dprintf("(parse) Invalid skipped data %2x\n",skip_data);
				//exit(1);
			//}
		}
	}

	stats_printf_audblk(audblk);
}

void
parse_auxdata(syncinfo_t *syncinfo,bitstream_t *bs)
{
	int i;
	int skip_length;
	uint_16 crc;
	uint_16 auxdatae;

	skip_length = (syncinfo->frame_size * 16)  - bs->total_bits_read - 17 - 1;

	//FIXME remove
	//dprintf("(auxdata) skipping %d auxbits\n",skip_length);
	
	for(i=0; i <  skip_length; i++)
		//printf("Skipped bit %i\n",(uint_16)bitstream_get(bs,1));
		bitstream_get(bs,1);

	//get the auxdata exists bit
	auxdatae = bitstream_get(bs,1);	

	//FIXME remove
	//dprintf("auxdatae = %i\n",auxdatae);

	//Skip the CRC reserved bit
	bitstream_get(bs,1);

	//Get the crc
	crc = bitstream_get(bs,16);
}


