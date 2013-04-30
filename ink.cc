#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/mxcfb.h>

#include "ink.h"

static int mxcfb = -1;

void Ink_UpdateRect(int x, int y, int w, int h){
	int marker = 999;
	struct mxcfb_update_data param;
	if(mxcfb < 0){
		mxcfb = open("/dev/fb0", O_RDWR);
	}
	if(mxcfb < 0){
		printf("mxcfb open error!\n");
	}

	param.update_region.left = x;
	param.update_region.top = y;
	param.update_region.width = w;
	param.update_region.height = h;
	param.waveform_mode = WAVEFORM_MODE_AUTO;
	param.update_mode = UPDATE_MODE_PARTIAL;
	param.update_marker = marker;
	param.temp = TEMP_USE_AMBIENT;
	param.flags = 0;

	if (ioctl(mxcfb, MXCFB_SEND_UPDATE, &param) == -1) {
		perror("Error sending update information");
		exit(1);
	}
	ioctl(mxcfb, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &marker);
}

void Ink_UpdateFull(){
	Ink_UpdateRect(0,0,600,800);
}

