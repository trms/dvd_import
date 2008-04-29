// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// jcinit.c:	part of the mjpgTools Library																			 //
//				This file contains initialization logic for the JPEG compressor.										 //
//				This routine is in charge of selecting the modules to be executed and making an initialization call		 //
//				to each one. Logically, this code belongs in jcmaster.c. It's split out because linking this routine	 //
//				implies linking the entire compression library. For a transcoding-only application, we want to be able	 //
//				to use jcmaster.c without linking in the whole library.													 //
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
// This sub-project is a corrected version of :																			 //
//  The sixth public release of the Independent JPEG Group's free JPEG software, release 6b of 27-Mar-1998				 //
//																														 //
//  Copyright (C) 1991-1998, Thomas G. Lane.																			 //
//  This software is the work of Tom Lane, Philip Gladstone, Jim Boucher, Lee Crocker, Julian Minguillon, Luis Ortiz,	 //
//  George Phillips, Davide Rossi, Guido Vollbeding, Ge' Weijers, and other members of the Independent JPEG Group.		 //
//																														 //
//  The authors make NO WARRANTY or representation, either express or implied, with respect to this software, its		 //
//  quality, accuracy, merchantability, or fitness for a particular purpose.  This software is provided "AS IS", and	 //
//  you, its user, assume the entire risk as to its quality and accuracy.												 //
//  For conditions of distribution and use, see the accompanying README file in the original package (jpeg-mmx.zip)		 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"


/*
 * Master selection of compression modules.
 * This is done once at the start of processing an image.  We determine
 * which modules will be used and give them appropriate initialization calls.
 */

GLOBAL(void)
jinit_compress_master (j_compress_ptr cinfo)
{
  /* Initialize master control (includes parameter checking/processing) */
  jinit_c_master_control(cinfo, FALSE /* full compression */);

  /* Preprocessing */
  if (! cinfo->raw_data_in) {
    jinit_color_converter(cinfo);
    jinit_downsampler(cinfo);
    jinit_c_prep_controller(cinfo, FALSE /* never need full buffer here */);
  }
  /* Forward DCT */
  jinit_forward_dct(cinfo);
  /* Entropy encoding: either Huffman or arithmetic coding. */
  if (cinfo->arith_code) {
    ERREXIT(cinfo, JERR_ARITH_NOTIMPL);
  } else {
    if (cinfo->progressive_mode) {
#ifdef C_PROGRESSIVE_SUPPORTED
      jinit_phuff_encoder(cinfo);
#else
      ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif
    } else
      jinit_huff_encoder(cinfo);
  }

  /* Need a full-image coefficient buffer in any multi-pass mode. */
  jinit_c_coef_controller(cinfo,
		(boolean) (cinfo->num_scans > 1 || cinfo->optimize_coding));
  jinit_c_main_controller(cinfo, FALSE /* never need full buffer here */);

  jinit_marker_writer(cinfo);

  /* We can now tell the memory manager to allocate virtual arrays. */
  (*cinfo->mem->realize_virt_arrays) ((j_common_ptr) cinfo);

  /* Write the datastream header (SOI) immediately.
   * Frame and scan headers are postponed till later.
   * This lets application insert special markers after the SOI.
   */
  (*cinfo->marker->write_file_header) (cinfo);
}
