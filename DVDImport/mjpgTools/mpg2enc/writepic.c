// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// writepic.c: write reconstructed pictures, part of the mjpgTools Library												 //
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
//#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include "global.h"

void writeframe(int frame_num, uint8_t *frame[])
{
#ifdef ORIGINAL_CODE
  int chrom_hsize, chrom_vsize;
  char name[128];
  FILE *fd;

  chrom_hsize = (chroma_format==CHROMA444) ? horizontal_size
                                           : horizontal_size>>1;

  chrom_vsize = (chroma_format!=CHROMA420) ? vertical_size
                                           : vertical_size>>1;

  if (fname[0]=='-')
    return;

  /* Y */
  sprintf(name,"%d.Y",frame_num);
  if (!(fd = fopen(name,"wb")))
  {
    mjpeg_error_exit1("Couldn't create %s",name);
  }
  fwrite(frame[0],1,horizontal_size*vertical_size,fd);
  fclose(fd);

  /* Cb */
  sprintf(name,"%s.U",fname);
  if (!(fd = fopen(name,"wb")))
  {
    mjpeg_error_exit1("Couldn't create %s",name);
  }
  fwrite(frame[1],1,chrom_hsize*chrom_vsize,fd);
  fclose(fd);

  /* Cr */
  sprintf(name,"%s.V",fname);
  if (!(fd = fopen(name,"wb")))
  {
    mjpeg_error_exit1("Couldn't create %s",name);
  }
  fwrite(frame[2],1,chrom_hsize*chrom_vsize,fd);
  fclose(fd);
#endif
}
