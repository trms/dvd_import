/*
 * AC3DEC_WAVE
 * -----------
 * v0.02 initial release
 *
 *  BBDMUX is the work of Brent Beyeler.
 *
 */

/***********************************************************************
*  bbdbits.h
*  Global Include Files
*
***********************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
//#include <mem.h>
#include <memory.h>

/* "bit_stream.h" Definitions */

#define DEMUX_MINIMUM         4    /* Minimum size of the buffer in bytes */
#define DEMUX_MAX_LENGTH      32   /* Maximum length of word written or
                                        read from bit stream */
#define DEMUX_READ_MODE       0
#define DEMUX_WRITE_MODE      1
#define DEMUX_ALIGNING        8
#define DEMUX_BINARY          0
#define DEMUX_ASCII           1
#define DEMUX_BS_FORMAT       BINARY /* BINARY or ASCII = 2x bytes */
#define DEMUX_BUFFER_SIZE     4096

/* "bit_stream.h" Type Definitions */

typedef struct bit_stream_struc
{
  FILE        *pt;            /* pointer to bit stream device */
  unsigned char *buf;         /* bit stream buffer */
  int         buf_size;       /* size of buffer (in number of bytes) */
  double      totbit;         /* bit counter of bit stream */
  int         buf_byte_idx;   /* pointer to top byte in buffer */
  int         buf_bit_idx;    /* pointer to top bit of top byte in buffer */
  int         mode;           /* bit stream open in read or write mode */
  int         eob;            /* end of buffer index */
  int         eobs;           /* end of bit stream flag */
  char        format;

    /* format of file in rd mode (BINARY/ASCII) */
} Bit_stream_struc;


/***********************************************************************
*
*  Global Function Prototype Declarations
*
***********************************************************************/

extern int            demux_memcheck(char*, int, int);
extern void           *demux_mem_alloc(unsigned long, char*);
extern void           demux_mem_free(void**);
extern void           demux_refill_buffer(Bit_stream_struc*);
extern void           demux_empty_buffer(Bit_stream_struc*, int);
extern void           demux_open_bit_stream_w(Bit_stream_struc*, const char*, int);
//extern void           demux_open_bit_stream_r(Bit_stream_struc*, const char*, int);
extern void           demux_open_bit_stream_r(Bit_stream_struc*, FILE*, int);
extern void           demux_close_bit_stream_r(Bit_stream_struc*);
extern void           demux_close_bit_stream_w(Bit_stream_struc*);
extern void           demux_alloc_buffer(Bit_stream_struc*, int);
extern void           demux_desalloc_buffer(Bit_stream_struc*);
extern void           demux_back_track_buffer(Bit_stream_struc*, int);
extern unsigned int   demux_get1bit(Bit_stream_struc*);
extern void           demux_put1bit(Bit_stream_struc*, int);
extern unsigned long  demux_look_ahead(Bit_stream_struc*, int);
extern unsigned long  demux_getbits(Bit_stream_struc*, int);
extern void           demux_putbits(Bit_stream_struc*, unsigned int, int);
extern void           demux_byte_ali_putbits(Bit_stream_struc*, unsigned int, int);
extern unsigned long  demux_byte_ali_getbits(Bit_stream_struc*, int);
extern double         demux_sstell(Bit_stream_struc*);
extern int            demux_end_bs(Bit_stream_struc*);
extern int            demux_seek_sync(Bit_stream_struc*, unsigned int, int);
extern size_t         s_fwrite( const void *buffer, size_t size, size_t count, FILE *stream );

extern long Feof( FILE *fp );