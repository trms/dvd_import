/*
 * downmix.h
 *
 * AC3DEC_WAVE
 * -----------
 *
 * header file for downmixing decoded multichannel streams to 
 * 2-channel WAVE data.
 */

#ifndef _DOWNMIX_H
#define _DOWNMIX_H 1

//#include "ac3.h"
//#include "decode.h"

void downmix( stream_samples_t *samples, sint_16 *out_samples, bsi_t *pbsi, int );
void conv480to441( sint_16 *out_samples, long samp_count, long chan );
#endif