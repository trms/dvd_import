//  bbdbits.cpp
//
// AC3DEC_WAVE
// -----------
// 
// v0.02 initial release
//  (originally from BBDMUX MPEG-2 demuxer package)
//
//  all fwrite() calls have been replaced with "s_fwrite()"
//  s_fwrite() stores data in the demux_buffer(), instead of writing to a
//  file.  Hence, s_fwrite() calls demux_buffer::push_data()
//

/*****************************************************************************
*
*  bit_stream.c package
*  Author:  Jean-Georges Fritsch, C-Cube Microsystems
*
*****************************************************************************/

/********************************************************************
  This package provides functions to write (exclusive or read)
  information from (exclusive or to) the bit stream.

  If the bit stream is opened in read mode only the get functions are
  available. If the bit stream is opened in write mode only the put
  functions are available.
********************************************************************/

/*open_bit_stream_w(); open the device to write the bit stream into it    */
/*demux_open_bit_stream_r(); open the device to read the bit stream from it     */
/*demux_close_bit_stream();  close the device containing the bit stream         */
/*demux_alloc_buffer();      open and initialize the buffer;                    */
/*demux_desalloc_buffer();   empty and close the buffer                         */
/*demux_back_track_buffer();     goes back N bits in the buffer                 */
/*unsigned int demux_get1bit();  read 1 bit from the bit stream                 */
/*unsigned long demux_getbits(); read N bits from the bit stream                */
/*unsigned long demux_byte_ali_getbits();   read the next byte aligned N bits from*/
/*                                    the bit stream                        */
/*unsigned long demux_look_ahead(); grep the next N bits in the bit stream without*/
/*                            changing the buffer pointer                   */
/*put1bit(); write 1 bit from the bit stream  */
/*put1bit(); write 1 bit from the bit stream  */
/*putbits(); write N bits from the bit stream */
/*byte_ali_putbits(); write byte aligned the next N bits into the bit stream*/
/*unsigned long demux_sstell(); return the current bit stream length (in bits)    */
/*int demux_end_bs(); return 1 if the end of bit stream reached otherwise 0       */
/*int demux_seek_sync(); return 1 if a sync word was found in the bit stream      */
/*                 otherwise returns 0                                      */


//put1bit()
//putbits()
//byte_ali_putbits()

#include "bbdbits.h"
#include "demuxbuf.h"

#define min(a, b)  (((a) < (b)) ? (a) : (b)) 

// g_demuxbuffer is declared in s_read.cpp
extern demux_buffer g_demuxbuffer;

// s_fwrite() replaces all instances of fwrite() in the original source
// code.  Currently, s_fwrite() calls demux_buffer::push_data(), storing
// the MPEG-2 (demuxed) stream data into our demux_buffer (instead of
// writing a file.)

// < CURRENT IMPLEMENTATION AT END OF FILE >

//size_t
//s_fwrite( const void *buffer, size_t size, size_t count, FILE *stream );
//{
	//return fwrite( buffer, size, count, stream );
//}



/*******************************************************************************
*
*  Allocate number of bytes of memory equal to "block".
*
*******************************************************************************/

void *demux_mem_alloc(
unsigned long block,
char *item)
{
  void *ptr;

  ptr = (void *) malloc((unsigned int)block);
  if (ptr != NULL)
    memset(ptr, 0, (unsigned int)block);
  else
  {
    printf("Unable to allocate %s\n", item);
    exit(1);
  }
  return(ptr);
}


/****************************************************************************
*
*  Free memory pointed to by "*ptr_addr".
*
*****************************************************************************/

void demux_mem_free(
void **ptr_addr)
{
  if (*ptr_addr != NULL)
  {
    free(*ptr_addr);
    *ptr_addr = NULL;
  }
}

/*******************************************************************************
*
*  Check block of memory all equal to a single byte, else return false
*
*******************************************************************************/

int demux_memcheck(
char *array,
int test,       /* but only tested as a char (bottom 8 bits) */
int num)
{
  int i = 0;

  while (array[i] == test && i<num)
    i++;
  if (i == num)
    return true;
  else
    return false;
}

extern "C" int m_allvobs;
extern "C" char m_inputfile[256];
extern "C" FILE* g_pInfile; /* declared in decode.c */

long bb_spanover(Bit_stream_struc *bs)
{
	char *p;
	// find vts_01_1.vob
	p = strchr( m_inputfile, '.' );
	if ( p ){
		p--;
		*p += 1;
		fclose( bs->pt );
		if ( g_pInfile = bs->pt = fopen( m_inputfile, "rb" ) ){
			printf( "\nSpanning over to %s\n", m_inputfile );
			return 1;
		} else
			printf( "\nNo span to %s\n", m_inputfile );
	}
	return 0;
}

#define PREREAD_SIZE	(1024*1024*1)


long FastReadChar( unsigned char *buffer, long size, FILE *fp )
{
static char *pre_readbuf = 0;
static long index = 0, buffsize=0, eof = 0;

	if ( !fp )
		eof = 1;

	if ( size == -2 ){	// check for EOF
		return eof;
	} else
	if ( size == 0 ){
		eof =
		index = buffsize = 0;
		return 0;
	}

	if ( pre_readbuf == 0 ) {
		pre_readbuf = (char*)malloc( PREREAD_SIZE+16 );
	}
	if ( index >= buffsize ){
		buffsize = fread( pre_readbuf, 1,PREREAD_SIZE, fp );
		//printf( "\nreading new buff = %d\n", buffsize);
		index = 0;
		if ( buffsize <= 0 ) {
			//printf( "\nEOF\n");
			eof = 1;
			index =	buffsize = 0;
			return 0;
		} else
			eof = 0;
	}
	if ( index < buffsize ){
		//printf( "reading 1char at %d\n", index);
		*buffer = *(pre_readbuf+index);
		index++;
	}
	//if ( index+1 == buffsize )
	//	eof = 1;
	return 1;
}

long Feof( FILE *fp )
{
	if ( fp )
		return FastReadChar(0,-2,fp);
	else
		return -1;
}

void ResetFastRead( void )
{
	FastReadChar(0,0,0);
}

/* refill the buffer from the input device when the buffer becomes empty    */
void demux_refill_buffer(
Bit_stream_struc *bs)   /* bit stream structure */
{
  register int i = bs->buf_size - 2 - bs->buf_byte_idx;
  register unsigned long n = 1;
  register int index = 0;
  char val[2];

  //printf( "***** demux_refill_buffer at %d\n", ftell(bs->pt));
  while ((i >= 0) && (!bs->eob))
  {
	if (bs->format == DEMUX_BINARY){
		//n = fread(&bs->buf[i--], sizeof(unsigned char), 1, bs->pt);
		n = FastReadChar( &bs->buf[i], 1, bs->pt );
		//printf( "fread %d at %d (i=%d,bufsize=%d,%d)\n", n, ftell(bs->pt),i, bs->buf_size,bs->buf_byte_idx );
		if ( Feof(bs->pt) && m_allvobs ) {
			if( bb_spanover( bs ) ){
				//FastReadChar( 0, -1, bs->pt );		// cancel EOF flag
				n = FastReadChar( &bs->buf[i], 1, bs->pt );
			}
		}
		i--;
    } else {
      while((index < 2) && n)
      {
		//printf( "fread %d at %d (i=%d,bufsize=%d,%d)\n", n, ftell(bs->pt),i, bs->buf_size,bs->buf_byte_idx );
        n = fread(&val[index], sizeof(char), 1, bs->pt);
        switch (val[index])
        {
          case 0x30:
          case 0x31:
          case 0x32:
          case 0x33:
          case 0x34:
          case 0x35:
          case 0x36:
          case 0x37:
          case 0x38:
          case 0x39:
          case 0x41:
          case 0x42:
          case 0x43:
          case 0x44:
          case 0x45:
          case 0x46:
            index++;
            break;
          default:
            break;
        }
      }

      if (val[0] <= 0x39)
        bs->buf[i] = (val[0] - 0x30) << 4;
      else
        bs->buf[i] = (val[0] - 0x37) << 4;
      if (val[1] <= 0x39)
        bs->buf[i--] |= (val[1] - 0x30);
      else
        bs->buf[i--] |= (val[1] - 0x37);
      index = 0;
    }

    if (!n)
      bs->eob = i + 1;
  }
}

#if BS_FORMAT != BINARY
static char *he = "0123456789ABCDEF";
#endif

/* empty the buffer to the output device when the buffer becomes full */
//void empty_buffer(
//Bit_stream_struc *bs,   /* bit stream structure */
//int minimum)            /* end of the buffer to empty */
//{
//  register int i;
//
//#if BS_FORMAT == BINARY
//  for (i = bs->buf_size - 1; i >= minimum; i--)
//    fwrite(&bs->buf[i], sizeof(unsigned char), 1, bs->pt);
//#else
//  for (i = bs->buf_size - 1; i >= minimum; i--)
//  {
//    char val[2];
//    val[0] = he[((bs->buf[i] >> 4) & 0x0F)];
//    val[1] = he[(bs->buf[i] & 0x0F)];
//    fwrite(val, sizeof(char), 2, bs->pt);
//  }
//#endif
//
//  for (i = minimum - 1; i >= 0; i--)
//    bs->buf[bs->buf_size - minimum + i] = bs->buf[i];

//  bs->buf_byte_idx = bs->buf_size -1 - minimum;
//  bs->buf_bit_idx = 8;
//}

/* open the device to write the bit stream into it */
//void open_bit_stream_w(
//Bit_stream_struc *bs,   /* bit stream structure */
//const char *bs_filenam,       /* name of the bit stream file */
//int size)               /* size of the buffer */
//{
//  if ((bs->pt = fopen(bs_filenam, "wb")) == NULL)
//  {
//    printf("Could not create \"%s\".\n", bs_filenam);
//    exit(1);
//  }
//  demux_alloc_buffer(bs, size);
//  bs->buf_byte_idx = size-1;
//  bs->buf_bit_idx = 8;
//  bs->totbit = 0;
//  bs->mode = WRITE_MODE;
//  bs->eob = false;
//  bs->eobs = false;
//}

extern "C" int m_lba_seek;


/* open the device to read the bit stream from it */
void demux_open_bit_stream_r(
Bit_stream_struc *bs,   /* bit stream structure */
FILE *fp_in,       /* pointer to FILE, input bitstream */
int size)               /* size of the buffer */
{
  register unsigned long n;
  register unsigned char flag = 1;
  unsigned char val;

  ResetFastRead();

  if ( fp_in != NULL )
	  bs->pt = fp_in; /* copy FILE pointer */
  else
  {
    fprintf( stderr, "\ndemux_open_bit_stream_r() : invalid *FILE pointer.");
    exit(1);
  }

  n = fseek( bs->pt, 0, SEEK_SET ); /* return to beginning of file */
  if ( n != 0 ) /* did fseek() succeed? */
  {
    fprintf(stderr,"\ndemux_open_bit_stream_r() : Could not seek -> pos 0.");
    exit(1);
  }

  do
  {
    n = fread(&val, sizeof(unsigned char), 1, bs->pt);
    switch (val)
    {
      case 0x30:
      case 0x31:
      case 0x32:
      case 0x33:
      case 0x34:
      case 0x35:
      case 0x36:
      case 0x37:
      case 0x38:
      case 0x39:
      case 0x41:
      case 0x42:
      case 0x43:
      case 0x44:
      case 0x45:
      case 0x46:
      case 0xa:  /* \n */
      case 0xd:  /* cr */
      case 0x1a:  /* sub */
        break;

      default: /* detection of an binary character */
        flag--;
        break;
    }
  } while (flag & n);

  if (flag)
    bs->format = DEMUX_ASCII;
  else
    bs->format = DEMUX_BINARY;

  n = fseek( bs->pt, m_lba_seek*2048, SEEK_SET ); /* return to beginning of file */
  if ( n != 0 ) /* did fseek() succeed? */
  {
    fprintf(stderr,"\ndemux_open_bit_stream_r() : Could not seek -> pos 0.");
    exit(1);
  }

  demux_alloc_buffer(bs, size);
  bs->buf_byte_idx = 0;
  bs->buf_bit_idx = 0;
  bs->totbit = 0;
  bs->mode = DEMUX_READ_MODE;
  bs->eob = false;
  bs->eobs = false;
}

/*close the device containing the bit stream after a read process*/
void demux_close_bit_stream_r(
Bit_stream_struc *bs)   /* bit stream structure */
{
	if ( bs->pt ){
		fclose(bs->pt);
	}
	bs->pt = 0;
	demux_desalloc_buffer(bs);
}

/*close the device containing the bit stream after a write process*/
//void close_bit_stream_w(
//Bit_stream_struc *bs)   /* bit stream structure */
//{
//  empty_buffer(bs, bs->buf_byte_idx);
//  fclose(bs->pt);
//  demux_desalloc_buffer(bs);
//}

/*open and initialize the buffer; */
void demux_alloc_buffer(
Bit_stream_struc *bs,   /* bit stream structure */
int size)
{
  bs->buf = (unsigned char *) demux_mem_alloc(size * sizeof(unsigned char), "buffer");
  bs->buf_size = size;
}

/*empty and close the buffer */
void demux_desalloc_buffer(
Bit_stream_struc *bs)   /* bit stream structure */
{
  free(bs->buf);
}

int demux_putmask[9] = {0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff};
int demux_clearmask[9] = {0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x0};

void demux_back_track_buffer( /* goes back N bits in the buffer */
Bit_stream_struc *bs,   /* bit stream structure */
int N)
{
  int tmp = N - (N / 8) * 8;
  register int i;

  bs->totbit -= N;
  for (i = bs->buf_byte_idx; i < bs->buf_byte_idx + N / 8 - 1; i++)
    bs->buf[i] = 0;
  bs->buf_byte_idx += N / 8;
  if ((tmp + bs->buf_bit_idx) <= 8)
    bs->buf_bit_idx += tmp;
  else
  {
    bs->buf_byte_idx ++;
    bs->buf_bit_idx += (tmp - 8);
  }
  bs->buf[bs->buf_byte_idx] &= demux_clearmask[bs->buf_bit_idx];
}

int demux_mask[8]={0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};

/*read 1 bit from the bit stream */
unsigned int demux_get1bit(
Bit_stream_struc *bs)   /* bit stream structure */
{
  unsigned int bit;
  register int i;

  bs->totbit++;
  if (!bs->buf_bit_idx)
  {
    bs->buf_bit_idx = 8;
    bs->buf_byte_idx--;
    if ((bs->buf_byte_idx < DEMUX_MINIMUM) || (bs->buf_byte_idx < bs->eob))
    {
      if (bs->eob)
        bs->eobs = true;
      else
      {
        for (i = bs->buf_byte_idx; i >= 0; i--)
          bs->buf[bs->buf_size - 1 - bs->buf_byte_idx+i] = bs->buf[i];
        demux_refill_buffer(bs);
        bs->buf_byte_idx = bs->buf_size-1;
      }
    }
  }
  bit = bs->buf[bs->buf_byte_idx] & demux_mask[bs->buf_bit_idx - 1];
  bit = bit >> (bs->buf_bit_idx - 1);
  bs->buf_bit_idx--;
  return(bit);
}

/*write 1 bit from the bit stream */
//void put1bit(
//Bit_stream_struc *bs,   /* bit stream structure */
//int bit)                /* bit to write into the buffer */
//{f
//  bs->totbit++;
//  bs->buf[bs->buf_byte_idx] |= (bit & 0x1) << (bs->buf_bit_idx - 1);
//  bs->buf_bit_idx--;
//  if (!bs->buf_bit_idx)
//  {
//    bs->buf_bit_idx = 8;
//    bs->buf_byte_idx--;
//    if (bs->buf_byte_idx < 0)
//      empty_buffer(bs, MINIMUM);
//    bs->buf[bs->buf_byte_idx] = 0;
//  }
//}

/*look ahead for the next N bits from the bit stream */
unsigned long demux_look_ahead(
Bit_stream_struc *bs,   /* bit stream structure */
int N)                  /* number of bits to read from the bit stream */
{
  unsigned long val = 0;
  register int j = N;
  register int k, tmp;
  register int bit_idx = bs->buf_bit_idx;
  register int byte_idx = bs->buf_byte_idx;

  if (N > DEMUX_MAX_LENGTH)
  {
    printf("Cannot read or write more than %d bits at a time.\n", 
		DEMUX_MAX_LENGTH);
    exit(1);
  }

  while (j > 0)
  {
    if (!bit_idx)
    {
      bit_idx = 8;
      byte_idx--;
    }
    k = min(j, bit_idx);
    tmp = bs->buf[byte_idx] & demux_putmask[bit_idx];
    tmp = tmp >> (bit_idx - k);
    val |= tmp << (j - k);
    bit_idx -= k;
    j -= k;
  }
  return(val);
}

/*read N bit from the bit stream */
unsigned long demux_getbits(
Bit_stream_struc *bs,   /* bit stream structure */
int N)                  /* number of bits to read from the bit stream */
{
  unsigned long val = 0;
  register int i;
  register int j = N;
  register int k, tmp;

  if (N > DEMUX_MAX_LENGTH)
  {
    printf("Cannot read or write more than %d bits at a time.\n", 
		DEMUX_MAX_LENGTH);
    exit(1);
  }
  bs->totbit += N;
  while (j > 0)
  {
    if (!bs->buf_bit_idx)
    {
      bs->buf_bit_idx = 8;
      bs->buf_byte_idx--;
      if ((bs->buf_byte_idx < DEMUX_MINIMUM) || (bs->buf_byte_idx < bs->eob))
      {
        if (bs->eob)
          bs->eobs = true;
        else
        {
          for (i = bs->buf_byte_idx; i >= 0; i--)
            bs->buf[bs->buf_size - 1 - bs->buf_byte_idx+i] = bs->buf[i];
          demux_refill_buffer(bs);
          bs->buf_byte_idx = bs->buf_size - 1;
        }
      }
    }
    k = min(j, bs->buf_bit_idx);
    tmp = bs->buf[bs->buf_byte_idx] & demux_putmask[bs->buf_bit_idx];
    tmp = tmp >> (bs->buf_bit_idx - k);
    val |= tmp << (j - k);
    bs->buf_bit_idx -= k;
    j -= k;
  }
  return(val);
}

/*write N bits into the bit stream */
//void putbits(
//Bit_stream_struc *bs,   /* bit stream structure */
//unsigned int val,       /* val to write into the buffer */
//int N)                  /* number of bits of val */
//{
//  register int j = N;
//  register int k, tmp;
//
//  if (N > MAX_LENGTH)
//  {
    //printf("Cannot read or write more than %d bits at a time.\n", MAX_LENGTH);
  //exit(1);
 //}
//
//  bs->totbit += N;
//  while (j > 0)
//  {
//    k = min(j, bs->buf_bit_idx);
//   tmp = val >> (j - k);
//    bs->buf[bs->buf_byte_idx] |= (tmp & demux_putmask[k]) << (bs->buf_bit_idx - k);
//    bs->buf_bit_idx -= k;
//    if (!bs->buf_bit_idx)
//    {
//      bs->buf_bit_idx = 8;
//      bs->buf_byte_idx--;
//      if (bs->buf_byte_idx < 0)
//        empty_buffer(bs, MINIMUM);
//      bs->buf[bs->buf_byte_idx] = 0;
//    }
//    j -= k;
// }
//}

/* do a mod on a floating point */
unsigned long demux_mod_double(double val, double modval)
{
  return (unsigned long) floor(val - floor(val / modval) * modval);
}


/*write N bits byte aligned into the bit stream */
//void byte_ali_putbits(
//Bit_stream_struc *bs,   /* bit stream structure */
//unsigned int val,       /* val to write into the buffer */
//int N)                  /* number of bits of val */
//{
//  unsigned long aligning;

//  if (N > MAX_LENGTH)
//  {
//    printf("Cannot read or write more than %d bits at a time.\n", MAX_LENGTH);
//    exit(1);
//  }

//  aligning = demux_mod_double(demux_sstell(bs), 8.0);
//  if (aligning)
//    putbits(bs, (unsigned int)0, (int)(8 - aligning));

//  putbits(bs, val, N);
//}

/*read the next bute aligned N bits from the bit stream */
unsigned long demux_byte_ali_getbits(
Bit_stream_struc *bs,   /* bit stream structure */
int N)                  /* number of bits of val */
{
  unsigned long aligning;

  if (N > DEMUX_MAX_LENGTH)
  {
    printf("Cannot read or write more than %d bits at a time.\n", 
		DEMUX_MAX_LENGTH);
    exit(1);
  }

  aligning = demux_mod_double(demux_sstell(bs), 8.0);
  if (aligning)
    demux_getbits(bs, (int)(8 - aligning));

  return (demux_getbits(bs, N));
}

/*return the current bit stream length (in bits)*/
double demux_sstell(
Bit_stream_struc *bs)   /* bit stream structure */
{
  return(bs->totbit);
}

/*return the status of the bit stream*/
/* returns 1 if end of bit stream was reached */
/* returns 0 if end of bit stream was not reached */
int demux_end_bs(
Bit_stream_struc *bs)   /* bit stream structure */
{
  return(bs->eobs);
}

static unsigned int val;

/*this function seeks for a byte aligned sync word in the bit stream and
  places the bit stream pointer right after the sync.
  This function returns 1 if the sync was found otherwise it returns 0  */
int demux_seek_sync(
Bit_stream_struc *bs,   /* bit stream structure */
unsigned int sync,      /* sync word maximum 32 bits */
int N)          /* sync word length */
{
  unsigned int aligning;
//  unsigned int val;
  unsigned int maxi = (int) pow(2.0, (double) N) - 1;

  aligning = demux_mod_double(demux_sstell(bs), (double) DEMUX_ALIGNING);
  if (aligning)
    demux_getbits(bs, (int)(DEMUX_ALIGNING - aligning));

  val = demux_getbits(bs, N);
  while (((val & maxi) != sync) && (!demux_end_bs(bs)))
  {
    val <<= DEMUX_ALIGNING;
    val |= demux_getbits(bs, DEMUX_ALIGNING);
  }

  if (demux_end_bs(bs))
    return(0);
  else
    return(1);
}

/*****************************************************************************
*
*  End of bit_stream.c package
*
*****************************************************************************/

/*  this version of s_fwrite() emulates the *fwrite()* command 
size_t
s_fwrite( const void *buffer, size_t size, size_t count, FILE *stream )
{
	return fwrite( buffer, size, count, stream );
}
*/


/*  moved to s_fread.cpp,

size_t
s_fwrite( const void *buffer, size_t size, size_t count, FILE *stream )
{
	long rc = 0;

	rc = g_demuxbuffer.push_data( (unsigned char *) buffer, size * count );

	if ( rc < 0 )
		rc = 0;

	return rc;
	
}
*/
