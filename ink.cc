#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/mxcfb.h>

#include "ink.h"

static int marker = 0;

static int mxcfb(){
	static int _fbfd = -1;
	if(_fbfd < 0)
		_fbfd = open("/dev/fb0", O_RDWR);
	if(_fbfd < 0){
		printf("error opening /dev/fb0!\n");
		exit(1);
	}
	return _fbfd;
}

void Ink_Init(){
	int update_scheme = UPDATE_SCHEME_QUEUE_AND_MERGE;
	ioctl(mxcfb(), MXCFB_WAIT_FOR_UPDATE_COMPLETE, &update_scheme);

	int yes = 1;
	ioctl(mxcfb(), MXCFB_SET_MERGE_ON_WAVEFORM_MISMATCH, &yes);
}

void Ink_Wait(){
	ioctl(mxcfb(), MXCFB_WAIT_FOR_UPDATE_COMPLETE, &marker);
}

static void _UpdateRect(int x, int y, int w, int h, int flags){
	struct mxcfb_update_data param;

	param.update_region.left = x;
	param.update_region.top = y;
	param.update_region.width = w;
	param.update_region.height = h;
	param.waveform_mode = (flags & INK_UPDATE_MERGE) ? 4 : WAVEFORM_MODE_AUTO;
	param.update_mode = (flags & INK_UPDATE_PARTIAL) ? UPDATE_MODE_PARTIAL : UPDATE_MODE_FULL;
	param.update_marker = ++marker;
	param.temp = TEMP_USE_AMBIENT;
	param.flags = 0;

	if (ioctl(mxcfb(), MXCFB_SEND_UPDATE, &param) == -1) {
		perror("Error sending update information");
		exit(1);
	}
}

void Ink_UpdateRect(int x, int y, int w, int h, int flags){
	_UpdateRect(x,y,w,h,flags);
}

void Ink_Update(int flags){
	_UpdateRect(0,0,600,800, flags);
}

