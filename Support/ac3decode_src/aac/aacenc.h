#define MBAACENC_DLL 

#define MAIN_PROFILE 0
#define LOW_PROFILE 1

#define VBR_NORMAL 0
#define VBR_LOW 1
#define VBR_HIGH 2

typedef struct {
	int channels;
	int sampling_rate;
	int bit_rate;
	int profile;
	int write_header;
	int use_VBR;
	int VBR_setting;
} mbAACConfig;

typedef struct {
	long total_bits;
	long frames;
	long cur_frame;
	int is_first_frame;
	int channels;
	int sampling_rate;
	int frame_bits;
	int available_bits;
	int write_header;
	int use_VBR;
	int profile;
	float **inputBuffer;
} mbAACStream;

typedef struct {
	int DLLMajorVersion;
	int DLLMinorVersion;
	int MajorVersion;
	int MinorVersion;
	char HomePage[255];
} mbVersion;

#ifndef MBAACENC_DLL

typedef mbAACStream* (*MBENCODEINIT) (mbAACConfig *ac, int *samplesToRead, int *bitBufferSize, int *headerSize);
typedef int (*MBENCODEFRAME) (mbAACStream *as, short *Buffer, int Samples, unsigned char *bitBuffer, int *bitBufSize);
typedef int (*MBENCODEFINISH) (mbAACStream *as, unsigned char *bitBuffer, int *bitBufSize);
typedef int (*MBENCODEFREE) (mbAACStream *as, unsigned char *headerBuf);
typedef mbVersion* (*MBENCODEVERSION) (void);

#define TEXT_MBENCODEINIT    "mbEncodeInit"
#define TEXT_MBENCODEFRAME   "mbEncodeFrame"
#define TEXT_MBENCODEFINISH  "mbEncodeFinish"
#define TEXT_MBENCODEFREE    "mbEncodeFree"
#define TEXT_MBENCODEVERSION "mbEncodeVersion"

#else

__declspec(dllexport) mbAACStream *mbEncodeInit(mbAACConfig *ac, int *samplesToRead, int *bitBufferSize, int *headerSize);
__declspec(dllexport) int mbEncodeFrame(mbAACStream *as, short *Buffer, int Samples, unsigned char *bitBuffer, int *bitBufSize);
__declspec(dllexport) int mbEncodeFree(mbAACStream *as, unsigned char *headerBuf);
__declspec(dllexport) int mbEncodeFinish(mbAACStream *as, unsigned char *bitBuffer, int *bitBufSize);
__declspec(dllexport) mbVersion *mbEncodeVersion(void);

#endif

