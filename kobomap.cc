
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <set>
#include <string>
#include <vector>

#include "SDL.h"

#define TILESIZE 256

#include "sdlmap.h"
#include "tile.h"
#include "tile_collection.h"
#include "map_view.h"
#include "tslib.h"
#include "ink.h"

static void resize(int width, int height){
	SDL_Surface *screen;
	fprintf(stderr, "setting video mode...\n");
	if(!(screen = SDL_SetVideoMode(width, height, 0, SDL_SWSURFACE | SDL_RESIZABLE))){
		fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
		exit(-1);
	}

	fprintf(stderr, "bpp: %i\n", screen->format->BitsPerPixel);
}

void tobw(SDL_Surface *surface){
	SDL_PixelFormat *fmt = surface->format;
	if(fmt->BitsPerPixel != 8){
		fprintf(stderr, "Not an 8-bit surface.\n");
		exit(-1);
	}
	Uint8 black = SDL_MapRGB(fmt, 0,  0,  0);
	Uint8 white = SDL_MapRGB(fmt, 255,255,255);
	Uint8 *pixels = (Uint8 *)surface->pixels;
	for(int idx = 0; idx < surface->h * surface->pitch; idx++){
		pixels[idx] = pixels[idx] < 128 ? black : white;
	}
}

void runloop(MapView &view){
	struct tsdev *ts;
	if(!(ts = ts_open("/dev/input/event1", 1))){
		perror("ts_open");
		exit(1);
	}
	if(ts_config(ts)){
		perror("ts_config");
		exit(1);
	}

	view.tiles.work();

	struct timeval lastclick;
	int lastx, lasty;
	lastx = lasty = -1;
	bool full = true;
	bool drag = false;
	for(;;){
		int maxfd = -1;

		struct ts_sample samples[64];
		int ret = ts_read(ts, &samples[0], 64);
		if(ret < 0){
			perror("ts_read");
			exit(1);
		}
		for(int i = 0; i < ret; i++){
			printf("%ld.%06ld: %6d %6d %6d\n", samples[i].tv.tv_sec, samples[i].tv.tv_usec, samples[i].x, samples[i].y, samples[i].pressure);
			if(drag){
				view.offsetx += lastx - samples[i].x;
				view.offsety += lasty - samples[i].y;
			}
			if(samples[i].pressure){
				if(!drag){
					/* start of drag */
					drag = full = true;
				}
			}else{
				unsigned int timediff = (samples[i].tv.tv_sec - lastclick.tv_sec) * 1000000 + (samples[i].tv.tv_usec - lastclick.tv_usec);
				if(timediff < 500000){
					view.zoom_at(samples[i].x, samples[i].y);
				}
				memcpy(&lastclick, &samples[i].tv, sizeof(struct timeval));
				drag = false;
				full = true;
			}
			lastx = samples[i].x;
			lasty = samples[i].y;
		}

		//if(drag && full){
		//	Ink_Wait();
		//	SDL_FillRect(SDL_GetVideoSurface(), NULL, SDL_MapRGB(SDL_GetVideoSurface()->format, 255, 255, 255));
		//	SDL_Flip(SDL_GetVideoSurface());
		//	Ink_Update(INK_UPDATE_FULL);
		//}

		view.update_bounds();
		view.tiles.work();
		view.render();

		if(drag)
			tobw(SDL_GetVideoSurface());
		Ink_Wait(); /* wait for display to update before drawing again */
		SDL_Flip(SDL_GetVideoSurface());

		if(full)
			Ink_Update(INK_UPDATE_FULL);
		else
			Ink_Update(drag ? INK_UPDATE_MERGE : INK_UPDATE_PARTIAL);

		printf("render!!!\n");
		full = false;
	}
}

int main(int argc, char *argv[]){
	if(SDL_Init(SDL_INIT_VIDEO)){
		fprintf(stderr, "SDL_Init failed: %s", SDL_GetError());
		exit(-1);
	}
	SDL_ShowCursor(0);
	int width = 600, height = 800;
	int zoom = 14;
	resize(width, height);
	MapView view(width, height, zoom);
	view.center_coords(48.4284, -123.3656);

	Ink_Init();

	runloop(view);
	return 0;
}

