// bbdmux.cpp
//
// AC3DEC_WAVE
// -----------
//  v0.02  initial release
//
//  The original BBDMUX executed in a while() loop until EOF.
//  Now, BBDMUX has been modified into demux_mpeg2(), which executes
//  a single time (or until *some* data has been extracted) and then
//  exits.
//
//  demux_ac3() calls s_fwrite(), which is rerouted to the g_ac3demuxQ
//  object.  BBDMUX calls s_fwrite() many times (transfering
//  just 1 byte per call.)  Multiple calls to s_fwrite() can cause the
//  program to malfunction.
//
//  This is due to the queue's FILLTHRESHOLD policy.  During a single
//  iteration of demux_ac3(), the queue may become full.  Some 
//  s_fwrite()'s will succeed and some will fail.  Thus not all of the
//  stream data is properly transfered to the queue.  This is a
//  *catastrophic* malfunction.
//
//  See demuxbuf.cpp for the current workaround.
//  
//  

/*************************************************************************
    bbDMUX by Brent Beyeler, beyeler@home.com
*************************************************************************/


#include "bbdbits.h"
#include <string.h>
#include <windows.h>
#include <stdio.h>


#undef SHOW_RESYNCHS

#define MPEG_PROGRAM_END_CODE    0x000001B9
#define PACK_START_CODE          0x000001BA
#define SYSTEM_HEADER_START_CODE 0x000001BB
#define PACKET_START_CODE_PREFIX 0x000001

#define PROGRAM_STREAM_MAP       0xBC
#define PRIVATE_STREAM_1         0xBD
#define PADDING_STREAM           0xBE
#define PRIVATE_STREAM_2         0xBF
#define ECM_STREAM               0xF0
#define EMM_STREAM               0xF1
#define PROGRAM_STREAM_DIRECTORY 0xFF
#define DSMCC_STREAM             0xF2
#define ITUTRECH222TYPEE_STREAM  0xF8

#define SUBSTREAM_AC3            0x80

/* demux_ac3() return codes */
#define DMX_OK	0
#define DMX_ENDOFSTREAM -1
#define DMX_NOTMPEG2 -2
#define VIDEO_STREAM_ID		0xE0
#define MAX_PACKET_LENGTH   16384



extern "C" int m_substream;
extern "C" int m_showsummary;

static Bit_stream_struc bs;
static unsigned long i, j, k, PES_packet_length, PES_header_data_length;
static unsigned long PTS_DTS_flags, ESCR_flag, ES_rate_flag, DSM_trick_mode_flag;
static unsigned long additional_copy_info_flag, PES_CRC_flag, PES_extension_flag;
static unsigned long PES_private_data_flag, pack_header_field_flag;
static unsigned long program_packet_sequence_counter_flag, PSTD_buffer_flag;
static unsigned long PES_extension_flag_2, PES_extension_field_length;
static unsigned long bytes_used, encrypted;
static int substream_to_get, stream_to_get, stream_id, substream_id;
static bool firsttime = true;
static unsigned long stream_packets[256], stream_bytes[256], pack_packets;
static unsigned long system_packets, ac3_bytes;
static unsigned long substream_packets[256], substream_bytes[256];
static bool streams_found[256];
static bool stream_encrypted[256];
static bool substream_encrypted[256];
static bool substreams_found[256];
static bool vob_flag;
static BOOL goto_nextpass = FALSE;	// marker for skipping 
static BOOL first_iteration = TRUE;




/* demux_ac3() is called by C-functions...thus we need to explicitly tell
   the C++ compiler to generate a backward compatible  function-call. */
extern "C" long demux_ac3( FILE *pInfile )
{

	unsigned char packet_buffer[ MAX_PACKET_LENGTH ];
	// local packet buffer, demux_ac3() fills packet_buffer[], then
	// dumps packet_buffer[]'s contents to fwrite() 
	// the buffering prevents writing of incomplete/broken packets to
	// the ac3demuxQ object
	long bCount = 0;

	if ( !pInfile ){
		bs.pt = 0;
	    demux_close_bit_stream_r(&bs);
		first_iteration = TRUE;
		firsttime = TRUE;
		goto_nextpass = FALSE;
		return 0;
	}


	if ( m_showsummary ){
		stream_to_get = -1;
		substream_to_get = -1;
	} else {
	    stream_to_get = PRIVATE_STREAM_1; /* AC3 stream */
		if ( m_substream )
			substream_to_get = m_substream;
		else
			substream_to_get = SUBSTREAM_AC3; /* AC3 stream */
	}

  if ( first_iteration )
  {
	first_iteration = FALSE;
	for (i = 0; i < 256; i++)
	{
	    streams_found[i] = false;
	    stream_packets[i] = 0;
	    stream_bytes[i] = 0;
	    substreams_found[i] = false;
	    stream_encrypted[i] = false;
	    substream_encrypted[i] = false;
		substream_packets[i] = 0;
		substream_bytes[i] = 0;
	}
	ac3_bytes = 0;
	pack_packets = 0;
	system_packets = 0;
//    outfp = NULL;
//	stream_to_get = -1;
//	substream_to_get = -1;

//  printf("bbDMUX - version 1.1, by Brent Beyeler (beyeler@home.com)\n\n");
//  if (argc < 2 || argc == 3)
//  {
//    printf("bbDUMX is an MPEG2 program stream de-multiplexor\n\n");
//    printf("Usage: bbDMUX  MPEG2 filename  <stream id  saved name>  <substream id>\n\n");
//    printf("Examples:\n");
//    printf("  To get a list of the stream and substream id's present in an MPEG2 file\n");
//    printf("    bbDMUX  testfile.mpg\n\n");
//    printf("  To save a video stream (id 0xe0) to the file video.m2v\n");
//    printf("    bbDMUX  testfile.m2p  0xe0  video.m2v\n\n");
//    printf("  To save an audio stream (id 0xc0) to the file audio.m2a\n");
//    printf("    bbDMUX  testfile.mpg  0xc0  audio.m2a\n\n");
//    printf("  To save a private 1 stream (id 0xbd) to the file private1.pvt\n");
//    printf("    bbDMUX  testfile.m2p  0xbd  private1.pvt\n\n");
//    printf("  To save an AC3 audio stream (substream 0x80) to the file audio.ac3\n");
//    printf("    bbDMUX  testfile.vob  0xbd  audio.ac3  0x80\n");
//	 exit (1);
//  }

//  demux_open_bit_stream_r (&bs, argv[1], BUFFER_SIZE);
    demux_open_bit_stream_r (&bs, pInfile, DEMUX_BUFFER_SIZE);
//  strcpy(tmpStr, src_name);
//  strlwr(tmpStr);
//  vob_flag = (strstr(tmpStr, ".vob") != NULL);
	vob_flag = 1; /* assume the pInfile is a *.VOB */
//	strcpy(tmpStr, src_name); /* record the filename */
//  if (argc > 2)
//  {
//    sscanf(argv[2], "0x%x", &i);
//    if (i < 0 || i > 0xFF)
//    {
//      printf("Stream ID must be in the range 0..0xFF\n");
//      exit (1);
//    }
//    if (argc > 3)
//    {
//      outfp = fopen(argv[3], "wb");
//      if (!outfp)
//      {
//        printf("Unable to open %s for output\n", argv[3]);
//        exit (1);
//      }
//    }
//    else
//    {
//      printf("An output filename must be specified\n");
//      exit(1);
//    }

//    if (argc > 4)
//    {
//      sscanf(argv[4], "0x%x", &i);
//      if (i < 0 || i > 0xFF)
//      {
//        printf("Substream ID must be in the range 0..0xFF\n");
//        exit (1);
//      }
//      substream_to_get = i;
//      vob_flag = true;
//    }
//  }
//  else
//    printf("Scanning for stream id's, press control-c to quit ...\n\n");

//  if (outfp)
//    if ((vob_flag) && (stream_to_get == PRIVATE_STREAM_1) && (substream_to_get > -1))
//      printf("Saving stream 0x%02X, substream 0x%02X into file %s...\n", stream_to_get, substream_to_get, argv[3]);
//    else
//      printf("Saving stream 0x%02X into file %s...\n", stream_to_get, argv[3]);

	i = demux_getbits(&bs, 32);

    if (i != PACK_START_CODE && i )
    {
        //printf("\nFile %s is not an MPEG2 Program Stream\n", src_name );
		fprintf( stderr, "\ndemux_ac3() : Infile is not an MPEG2 Program Stream");
        //exit(1);
		return DMX_NOTMPEG2;
    }
    goto_nextpass = TRUE; /* skip the following statement i=demux_getbits() */
    
  } // endif --------- if ( first_iteration ) ...

  do { 
	if ( goto_nextpass == FALSE )  /* skip this operation */
	  i = demux_getbits(&bs, 32);
	goto_nextpass = FALSE;	/* reset flag */

    switch (i)
    {
      case PACK_START_CODE:
        pack_packets++;
        demux_getbits(&bs, 32);   // pack header
        demux_getbits(&bs, 32);
        demux_getbits(&bs, 13);
        i = demux_getbits(&bs, 3);
        for (j = 0; j < i; j++)
          demux_getbits(&bs, 8);  // stuffing bytes
        break;

      case SYSTEM_HEADER_START_CODE:
        system_packets++;
        demux_getbits(&bs, 32);   // system header
        demux_getbits(&bs, 32);
        while (demux_look_ahead(&bs, 1) == 1)
          demux_getbits(&bs, 24); // P-STD info
        break;

      case MPEG_PROGRAM_END_CODE:
        break;

      default:
        if ((i >> 8) != PACKET_START_CODE_PREFIX)
        {
#ifdef SHOW_RESYNCHS
//          if (argc > 3)
//          {
//            printf("\nUnknown bits in stream = 0x%08X at byte %.0f\n", i, sstell(&bs) / 8 - 4);
//            printf("\nAttempting resynch ... ");
//          }
#endif
          if (demux_seek_sync(&bs, PACKET_START_CODE_PREFIX, 24))
          {
            i = 0x00000100 | demux_getbits(&bs, 8);
#ifdef SHOW_RESYNCHS
//            if (argc > 3)
//              printf("succesfully resynched\n");
#endif
            //goto nextpass;
			goto_nextpass = TRUE;
			//return DMX_OK;
			return bCount;
          }
#ifdef SHOW_RESYNCHS
//          if (argc > 3)
//            printf("could not resynch\n");
//          else
#endif
//            printf("\nUnknown bits in stream = 0x%08X at byte %.0f\n", i, sstell(&bs) / 8 - 4);
          //exit(1);
        }
        stream_id = i & 0x000000FF;
        stream_packets[stream_id]++;

        PES_packet_length = demux_getbits(&bs, 16);
        encrypted = 0;
        switch (stream_id)
        {
          case PROGRAM_STREAM_MAP:
          case ECM_STREAM:
          case EMM_STREAM:
          case PROGRAM_STREAM_DIRECTORY:
          case DSMCC_STREAM:
          case ITUTRECH222TYPEE_STREAM:
          case PRIVATE_STREAM_2:
          case PADDING_STREAM:
            if (stream_id == stream_to_get)
            {
              for (j = 0; j < PES_packet_length; j++)
              {
                k = demux_getbits(&bs, 8);
                //if (s_fwrite(&k, 1, 1, outfp) != 1)
				if (s_fwrite(&k, 1, 1, NULL) != 1)
                {
                  //printf("Error writing to output file %s.", argv[3]);
				  fprintf( stderr, 
					  "\ndemux_ac3() : Error writing to stream input buffer." );
                  exit (1);
                }
				else
					bCount += 1;
              }
            }
            for (j = 0; j < PES_packet_length; j++)
              demux_getbits(&bs, 8);
            stream_bytes[stream_id] += j;
            break;

          default:
            demux_getbits(&bs, 2);
            encrypted = demux_getbits(&bs, 2);   // PES_scrambling_control
            demux_getbits(&bs, 4);
            PTS_DTS_flags = demux_getbits(&bs, 2);
            ESCR_flag = demux_get1bit(&bs);
            ES_rate_flag = demux_get1bit(&bs);
            DSM_trick_mode_flag = demux_get1bit(&bs);
            additional_copy_info_flag = demux_get1bit(&bs);
            PES_CRC_flag = demux_get1bit(&bs);
            PES_extension_flag = demux_get1bit(&bs);
            PES_header_data_length = demux_getbits(&bs, 8);
            bytes_used = 0;
            if (PTS_DTS_flags > 1)
            {
              demux_getbits(&bs, 32);   // PTS stamp
              demux_getbits(&bs, 8);
              bytes_used += 5;
            }
            if (PTS_DTS_flags > 2)
            {
              demux_getbits(&bs, 32);   // DTS stamp
              demux_getbits(&bs, 8);
              bytes_used += 5;
            }

            if (ESCR_flag == 1)
            {
              demux_getbits(&bs, 32);
              demux_getbits(&bs, 16);
              bytes_used += 6;
            }

            if (ES_rate_flag == 1)
            {
              demux_getbits(&bs, 24);
              bytes_used += 3;
            }

            if (DSM_trick_mode_flag == 1)
            {
              demux_getbits(&bs, 8);
              bytes_used++;
            }

            if (additional_copy_info_flag == 1)
            {
              demux_getbits(&bs, 8);
              bytes_used++;
            }

            if (PES_CRC_flag == 1)
            {
              demux_getbits(&bs, 16);
              bytes_used += 2;
            }

            if (PES_extension_flag == 1)
            {
              PES_private_data_flag = demux_get1bit(&bs);
              pack_header_field_flag = demux_get1bit(&bs);
              program_packet_sequence_counter_flag = demux_get1bit(&bs);
              PSTD_buffer_flag = demux_get1bit(&bs);
              demux_getbits(&bs, 3);
              PES_extension_flag_2 = demux_get1bit(&bs);
              bytes_used++;

              if (PES_private_data_flag == 1)
              {
                for (j = 0; j < 128; j++)
                  demux_getbits(&bs, 8);
                bytes_used += 128;
              }

              if (pack_header_field_flag == 1)
              {
                fprintf( stderr, "\ndemux_ac3() : pack header field flag value not allowed in program streams\n");
                exit(1);
              }

              if (program_packet_sequence_counter_flag == 1)
              {
                demux_getbits(&bs, 16);
                bytes_used += 2;
              }

              if (PSTD_buffer_flag == 1)
              {
                demux_getbits(&bs, 16);
                bytes_used += 2;
              }

              if (PES_extension_flag_2 == 1)
              {
                demux_get1bit(&bs);
                PES_extension_field_length = demux_getbits(&bs, 7);
                bytes_used++;
                for (j = 0; j < PES_extension_field_length; j++)
                {
                  demux_getbits(&bs, 8);
                  bytes_used++;
                }
              }
            }

            for (j = 0; j < PES_header_data_length - bytes_used; j++)
              demux_getbits(&bs, 8);

            substream_id = demux_getbits(&bs, 8);
            stream_bytes[stream_id] += PES_packet_length - PES_header_data_length - 3;
            if ((stream_id == PRIVATE_STREAM_1) && (vob_flag))
            {
              substream_packets[substream_id]++;
              substream_bytes[substream_id] += PES_packet_length - PES_header_data_length - 3;
              if (substream_id == SUBSTREAM_AC3)
                ac3_bytes += PES_packet_length - PES_header_data_length - 7;
            }
            if (stream_id == stream_to_get)
            {
              if ((stream_id == PRIVATE_STREAM_1) && (vob_flag) && (substream_to_get > -1))
              {
                if (substream_id == substream_to_get)
                {
					if (substream_id == substream_to_get){
                      demux_getbits(&bs, 24);  // discard the 4 AC3 ident bytes
					} else {
                      //if (s_fwrite(&substream_id, 1, 1, outfp) != 1)
					  if (s_fwrite(&substream_id, 1, 1, NULL) != 1)
                      {
                        //printf("Error writing to output file %s.", argv[3]);
						fprintf( stderr, "\ndemux_ac3() : Error writing to stream input buffer." );
                        exit (1);
                      }
					  else
						bCount += 1;
                      for (j = 0; j < 3; j++)
                      {
                        k = demux_getbits(&bs, 8);
                        //if (s_fwrite(&k, 1, 1, outfp) != 1)
						if (s_fwrite(&k, 1, 1, NULL) != 1)
                        {
                          //printf("Error writing to output file %s.", argv[3]);
						  fprintf( stderr, "\ndemux_ac3() : Error writing to stream input buffer." );
                          exit (1);
                        }
						else
							bCount += 1;
                      }
					}

/* AC3DEC_WAVE - changed the for-loop to detect EOF condition.  Does not write 
   *incomplete* packets to ac3demuxQ.  Reduces ac3dec's chance of crashing
   at the termination of the bitstream! */
                  for (j = 0; ( (j < PES_packet_length - PES_header_data_length - 7)
					  && ( !Feof(bs.pt)) ) ; j++)
				  {
                    k = demux_getbits(&bs, 8);
					packet_buffer[ j ] = k; // store packet in our local buffer
                    //if (s_fwrite(&k, 1, 1, outfp) != 1)
				  }
				  // j = number of valid bytes in packet_buffer 
				  if ( Feof(bs.pt) && ( j < PES_packet_length - PES_header_data_length - 7) )
					  /* end of file, *AND* fewer bytes written ? */
					  fprintf(stderr,"\ndemux_ac3() : discarding incomplete packet, only %lu bytes read, ", j );
				  else
				  {	/* we have a *full* packet to write */
				    if (s_fwrite(&packet_buffer, 1, j, NULL) != j)
                    {
                      //printf("Error writing to output file %s.", argv[3]);
					  fprintf( stderr, "\ndemux_ac3() : Error writing to stream input buffer." );
                      exit (1);
                    }
					else
						bCount += j;
				  } /* endif ( bs->eobs == false ) */

                }
                else
                  for (j = 0; j < PES_packet_length - PES_header_data_length - 4; j++)
                    demux_getbits(&bs, 8);
              }
              else
              {
                //if (s_fwrite(&substream_id, 1, 1, outfp) != 1)
				if (s_fwrite(&substream_id, 1, 1, NULL) != 1)
                {
				  fprintf( stderr, "\ndemux_ac3() : Error writing to stream input buffer." );
                  //printf("Error writing to output file %s.", argv[3]);
                  exit (1);
                }
				else
					bCount += 1;
                for (j = 0; j < PES_packet_length - PES_header_data_length - 4; j++)
                {
		          k = demux_getbits(&bs, 8);
                  //if (s_fwrite(&k, 1, 1, outfp) != 1)
				  if (s_fwrite(&k, 1, 1, NULL) != 1)
                  {
                    //printf("Error writing to output file %s.", argv[3]);
					fprintf( stderr, "\ndemux_ac3() : Error writing to stream input buffer." );
                    exit (1);
                  }
				  else
					bCount += 1;
                }
              }
            }
            else
              for (j = 0; j < PES_packet_length - PES_header_data_length - 4; j++)
                demux_getbits(&bs, 8);
            break;
        }

        
		// --------------------------------------------------------
		if ( m_showsummary )
        {
          if ((!streams_found[stream_id]) || ((vob_flag) && (stream_id == PRIVATE_STREAM_1) && (!substreams_found[substream_id])))
          {
            switch (stream_id)
            {
              case PROGRAM_STREAM_MAP:
                printf("Found stream id 0x%02X  = Program Stream Map\n", stream_id);
                break;
              case PRIVATE_STREAM_1:
                if (vob_flag)
                {
                  switch (substream_id)
                  {
                    case SUBSTREAM_AC3:
                      printf("Found stream id 0x%02X  = Private Stream 1, substream 0x80, AC3 audio\n", stream_id);
                      break;
                    default:
                      printf("Found stream id 0x%02X  = Private Stream 1, substream 0x%02X\n", stream_id, substream_id);
                      break;
                  }
                  substreams_found[substream_id] = true;
                }
                else
                  printf("Found stream id 0x%02X  = Private Stream 1\n", stream_id);
                break;
              case PRIVATE_STREAM_2:
                printf("Found stream id 0x%02X  = Private Stream 2\n", stream_id);
                break;
              case ECM_STREAM:
                printf("Found stream id 0x%02X  = ECM Stream\n", stream_id);
                break;
              case EMM_STREAM:
                printf("Found stream id 0x%02X  = EMM Stream\n", stream_id);
                break;
              case PROGRAM_STREAM_DIRECTORY:
                printf("Found stream id 0x%02X  = Program Stream Directory\n", stream_id);
                break;
              case DSMCC_STREAM:
                printf("Found stream id 0x%02X  = DSMCC Stream\n", stream_id);
                break;
              case ITUTRECH222TYPEE_STREAM:
                printf("Found stream id 0x%02X  = ITU-T Rec. H.222.1 type E\n", stream_id);
                break;
              case PADDING_STREAM:
                printf("Found stream id 0x%02X  = Padding Stream\n", stream_id);
                break;
              default:
                if ((stream_id >= 0xE0) && (stream_id <= 0xEF))
                  printf("Found stream id 0x%02X  = Video Stream %d\n", stream_id, stream_id - 0xE0);
                else
                  if ((stream_id >= 0xC0) && (stream_id <= 0xDF))
                    printf("Found stream id 0x%02X  = Audio Stream %d\n", stream_id, stream_id - 0xC0);
                  else
                    printf("Found stream id 0x%02X  = other stream\n", stream_id);
            }
            streams_found[stream_id] = true;
          }
          if ((stream_id == PRIVATE_STREAM_1) && (vob_flag))
          {
            if (encrypted)
            {
              if (!substream_encrypted[substream_id])
              {
                printf("Stream id 0x%02X, substream 0x%02X is encrypted\n", stream_id, substream_id);
                substream_encrypted[substream_id] = true;
              }
            }
          }
          else
          {
            if (encrypted)
            {
              if (!stream_encrypted[stream_id])
              {
                printf("Stream id 0x%02X is encrypted\n", stream_id);
                stream_encrypted[stream_id] = true;
              }
            }
          }
        } // endif
    } //switch (i)
  // Is the stream finished?
  } while ( (i != MPEG_PROGRAM_END_CODE) && ( !Feof(bs.pt)) && (bCount == 0) );

  if ( m_showsummary && !first_iteration ){
	  printf( "\n");
  } else
  if ( bCount ) return bCount;  // no it's not finished

  // stream is done, now generate some statistics
  printf( "\ndemux_ac3() : BBDMUX Summary:\n-----------------------------");
  printf("\nMPEG2 Packs = %d, PES_packet_length = %lu ", pack_packets, 
	  PES_packet_length);
  if (system_packets)
    printf("System headers = %d\n", system_packets);
  for (i = 0; i < 256; i++)
  {
    if (stream_packets[i])
    {
      switch (i)
      {
        case PROGRAM_STREAM_MAP:
          printf("Program Stream Map packets = %d, total bytes = %d\n", stream_packets[i], stream_bytes[i]);
          break;
        case PRIVATE_STREAM_1:
          printf("Private Stream 1 packets = %d, total bytes = %d\n", stream_packets[i], stream_bytes[i]);
          if (vob_flag)
          {
            for (j = 0; j < 256; j++)
            {
              if (substream_packets[j])
              {
                switch (j)
                {
                  case SUBSTREAM_AC3:
                    printf("AC3 Audio substream packets = %d, total bytes = %d\n", substream_packets[j], ac3_bytes);
                    break;
                  default:
                    printf("Substream 0x%02X packets = %d, total bytes = %d\n", j, substream_packets[j], substream_bytes[j]);
                    break;
                }
              }
            }
          }
          break;
        case PRIVATE_STREAM_2:
          printf("Private Stream 2 packets = %d, total bytes = %d\n", stream_packets[i], stream_bytes[i]);
          break;
        case ECM_STREAM:
          printf("ECM Stream packets = %d, total bytes = %d\n", stream_packets[i], stream_bytes[i]);
          break;
        case EMM_STREAM:
          printf("EMM Stream packets = %d, total bytes = %d\n", stream_packets[i], stream_bytes[i]);
          break;
        case PROGRAM_STREAM_DIRECTORY:
          printf("Program Stream Directory packets = %d, total bytes = %d\n", stream_packets[i], stream_bytes[i]);
          break;
        case DSMCC_STREAM:
          printf("DSMCC Stream packets = %d, total bytes = %d\n", stream_packets[i], stream_bytes[i]);
          break;
        case ITUTRECH222TYPEE_STREAM:
          printf("ITU-T Rec. H.222.1 type E packets = %d, total bytes = %d\n", stream_packets[i], stream_bytes[i]);
          break;
        case PADDING_STREAM:
          printf("Padding Stream packets = %d, total bytes = %d\n", stream_packets[i], stream_bytes[i]);
          break;
        default:
          if ((i >= 0xE0) && (i <= 0xEF))
            printf("Video stream %d packets = %d, total bytes = %d\n", i - 0xE0, stream_packets[i], stream_bytes[i]);
          else
            if ((i >= 0xC0) && (i <= 0xDF))
              printf("Audio stream %d packets = %d, total bytes = %d\n", i - 0xC0, stream_packets[i], stream_bytes[i]);
            else
              printf("Other stream 0x%02X packets = %d, total bytes = %d\n", i, stream_packets[i], stream_bytes[i]);
      }
    }
  }

  demux_close_bit_stream_r(&bs);

  first_iteration = TRUE;
  firsttime = TRUE;
  goto_nextpass = FALSE;

  //if (outfp)
  //  fclose(outfp);
  return DMX_ENDOFSTREAM;
}


