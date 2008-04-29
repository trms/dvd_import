// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// cdjpeg.c:	part of the mjpgTools Library																			 //
//				This file contains common support routines used by the IJG application programs (cjpeg, djpeg, jpegtran).//
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
//   the Independent JPEG Group's software																				 //
//																														 //
//	Copyright (C) 1991-1997, Thomas G. Lane.																			 //
//  The authors make NO WARRANTY or representation, either express or implied, with respect to this software, its		 //
//  quality, accuracy, merchantability, or fitness for a particular purpose.  This software is provided "AS IS", and	 //
//  you, its user, assume the entire risk as to its quality and accuracy.												 //
//  For conditions of distribution and use, see the accompanying README file in the original package (jpeg-mmx.zip)		 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#include "cdjpeg.h"		/* Common decls for cjpeg/djpeg applications */
#include <ctype.h>		/* to declare isupper(), tolower() */
#ifdef NEED_SIGNAL_CATCHER
#include <signal.h>		/* to declare signal() */
#endif
#ifdef USE_SETMODE
#include <fcntl.h>		/* to declare setmode()'s parameter macros */
/* If you have setmode() but not <io.h>, just delete this line: */
#include <io.h>			/* to declare setmode() */
#endif

#include "tools.h"		// for function 'Message'

/*
 * Signal catcher to ensure that temporary files are removed before aborting.
 * NB: for Amiga Manx C this is actually a global routine named _abort();
 * we put "#define signal_catcher _abort" in jconfig.h.  Talk about bogus...
 */

#ifdef NEED_SIGNAL_CATCHER

static j_common_ptr sig_cinfo;

void				/* must be global for Manx C */
signal_catcher (int signum)
{
  if (sig_cinfo != NULL) {
    if (sig_cinfo->err != NULL) /* turn off trace output */
      sig_cinfo->err->trace_level = 0;
    jpeg_destroy(sig_cinfo);	/* clean up memory allocation & temp files */
  }
  exit(EXIT_FAILURE);
}


GLOBAL(void)
enable_signal_catcher (j_common_ptr cinfo)
{
  sig_cinfo = cinfo;
#ifdef SIGINT			/* not all systems have SIGINT */
  signal(SIGINT, signal_catcher);
#endif
#ifdef SIGTERM			/* not all systems have SIGTERM */
  signal(SIGTERM, signal_catcher);
#endif
}

#endif


/*
 * Optional progress monitor: display a percent-done figure on stderr.
 */

#ifdef PROGRESS_REPORT

METHODDEF(void)
progress_monitor (j_common_ptr cinfo)
{
  cd_progress_ptr prog = (cd_progress_ptr) cinfo->progress;
  int total_passes = prog->pub.total_passes + prog->total_extra_passes;
  int percent_done = (int) (prog->pub.pass_counter*100L/prog->pub.pass_limit);

  if (percent_done != prog->percent_done) {
    prog->percent_done = percent_done;
    if (total_passes > 1) {
      fprintf(stderr, "\rPass %d/%d: %3d%% ",
	      prog->pub.completed_passes + prog->completed_extra_passes + 1,
	      total_passes, percent_done);
    } else {
      fprintf(stderr, "\r %3d%% ", percent_done);
    }
    fflush(stderr);
  }
}


GLOBAL(void)
start_progress_monitor (j_common_ptr cinfo, cd_progress_ptr progress)
{
  /* Enable progress display, unless trace output is on */
  if (cinfo->err->trace_level == 0) {
    progress->pub.progress_monitor = progress_monitor;
    progress->completed_extra_passes = 0;
    progress->total_extra_passes = 0;
    progress->percent_done = -1;
    cinfo->progress = &progress->pub;
  }
}


GLOBAL(void)
end_progress_monitor (j_common_ptr cinfo)
{
  /* Clear away progress display */
  if (cinfo->err->trace_level == 0) {
    fprintf(stderr, "\r                \r");
    fflush(stderr);
  }
}

#endif


/*
 * Case-insensitive matching of possibly-abbreviated keyword switches.
 * keyword is the constant keyword (must be lower case already),
 * minchars is length of minimum legal abbreviation.
 */

GLOBAL(boolean)
keymatch (char * arg, const char * keyword, int minchars)
{
  register int ca, ck;
  register int nmatched = 0;

  while ((ca = *arg++) != '\0') {
    if ((ck = *keyword++) == '\0')
      return FALSE;		/* arg longer than keyword, no good */
    if (isupper(ca))		/* force arg to lcase (assume ck is already) */
      ca = tolower(ca);
    if (ca != ck)
      return FALSE;		/* no good */
    nmatched++;			/* count matched characters */
  }
  /* reached end of argument; fail if it's too short for unique abbrev */
  if (nmatched < minchars)
    return FALSE;
  return TRUE;			/* A-OK */
}


/*
 * Routines to establish binary I/O mode for stdin and stdout.
 * Non-Unix systems often require some hacking to get out of text mode.
 */

GLOBAL(FILE *)
read_stdin (void)
{
  FILE * input_file = stdin;

#ifdef USE_SETMODE		/* need to hack file mode? */
  setmode(fileno(stdin), O_BINARY);
#endif
#ifdef USE_FDOPEN		/* need to re-open in binary mode? */
	if ((input_file = fdopen(fileno(stdin), READ_BINARY)) == NULL)
	{ // modified
		Message("Cannot reopen stdin");
		// fprintf(stderr, "Cannot reopen stdin\n");
		exit(EXIT_FAILURE);
	}
#endif
  return input_file;
}


GLOBAL(FILE *)
write_stdout (void)
{
  FILE * output_file = stdout;

#ifdef USE_SETMODE		/* need to hack file mode? */
  setmode(fileno(stdout), O_BINARY);
#endif
#ifdef USE_FDOPEN		/* need to re-open in binary mode? */
	if ((output_file = fdopen(fileno(stdout), WRITE_BINARY)) == NULL)
	{
		Message("Cannot reopen stdout");
		//fprintf(stderr, "Cannot reopen stdout\n");
		exit(EXIT_FAILURE);
	}
#endif
  return output_file;
}
