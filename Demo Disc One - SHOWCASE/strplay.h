#ifndef STRPLAY_H
#define STRPLAY_H


#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>

#define IS_RGB24	0	// 0:16-bit playback, 1:24-bit playback (recommended for quality)
#define RING_SIZE	10	// Ring Buffer size (32 sectors seems good enough)

#if IS_RGB24==1
	#define PPW			3/2	// pixels per short word
	#define DCT_MODE	3	// Decode mode for DecDCTin routine
#else
	#define PPW			1
	#define DCT_MODE    2
#endif




// A simple struct to make STR handling a bit easier
typedef struct {
	char	FileName[32];
	int		Xres;
	int		Yres;
	int		NumFrames;
} STRFILE;

// Decode environment
typedef struct {
	u_long	*VlcBuff_ptr[2];	// Pointers to the VLC buffers
	u_short	*ImgBuff_ptr[2];	// Pointers to the frame slice buffers
	RECT	rect[2];			// VRAM parameters on where to draw the frame data to
	RECT	slice;				// Frame slice parameters for loading into VRAM
	int		VlcID;				// Current VLC buffer ID
	int		ImgID;				// Current slice buffer ID
	int 	RectID;				// Current video buffer ID
	int		FrameDone;			// Frame decode completion flag
} STRENV;

// A bunch of internal variables
static STRENV strEnv;

static int	strScreenWidth=0,strScreenHeight=0;
static int	strFrameX=0,strFrameY=0;
static int	strNumFrames=0;

static int strFrameWidth=0,strFrameHeight=0;	// Frame size of STR file
static int strPlayDone=0;						// Playback completion flag

// Main function prototypes
int PlayStr(int xres, int yres, int xpos, int ypos, STRFILE *str);

// Internal function prototypes
static void strDoPlayback(STRFILE *str);
static void strCallback();
static void strNextVlc(STRENV *strEnv);
static void strSync(STRENV *strEnv, int mode);
static u_long *strNext(STRENV *strEnv);
static void strKickCD(CdlLOC *loc);

#endif
