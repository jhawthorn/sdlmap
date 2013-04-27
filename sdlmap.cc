
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

struct Coordinate{
	double lat, lng;
};

struct Point{
	double x, y;
	Point(Coordinate &c):
		x((c.lng + 180.0f) / 360.0f),
		y((1.0f - log(tan(c.lat * M_PI / 180.0f) + 1.0f / cos(c.lat * M_PI / 180.0f)) / M_PI) / 2.0f) {}
	Point &operator<<=(int zoom){
		x *= (1 << zoom);
		y *= (1 << zoom);
		return *this;
	}
};

static void resize(int width, int height){
	if(!SDL_SetVideoMode(width, height, 0, SDL_SWSURFACE | SDL_RESIZABLE)){
		fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
		exit(-1);
	}
}

void runloop(MapView &view){
	bool mousedown = false;
	bool dirty = true;
	Uint32 lastclick = 0;
	for(;;){
		SDL_Event event;
		while((dirty ? SDL_PollEvent : SDL_WaitEvent)(&event)){
			switch (event.type) {
				case SDL_MOUSEBUTTONUP:
					mousedown = false;
					break;
				case SDL_MOUSEBUTTONDOWN:
					switch(event.button.button){
						case 4:
							view.zoom_at(event.button.x, event.button.y);
							dirty = true;
							break;
						case 5:
							view.zoom_out();
							dirty = true;
							break;
						default:
							mousedown = true;
							if(SDL_GetTicks() - lastclick < 250){
								lastclick = 0;
								view.zoom_at(event.button.x, event.button.y);
								dirty = true;
							}
							lastclick = SDL_GetTicks();
					}
					break;
				case SDL_MOUSEMOTION:
					if(mousedown){
						view.offsetx -= event.motion.xrel;
						view.offsety -= event.motion.yrel;
						dirty = true;
					}
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym){
						case SDLK_ESCAPE:
							exit(0);
							break;
						case SDLK_PLUS:
						case SDLK_EQUALS:
							view.zoom_in();
							dirty = true;
							break;
						case SDLK_MINUS:
							view.zoom_out();
							dirty = true;
							break;
					}
					break;
				case SDL_VIDEORESIZE:
					resize(event.resize.w, event.resize.h);
					view.resize(event.resize.w, event.resize.h);
					dirty = true;
					break;
				case SDL_QUIT:
					exit(0);
			}
		}
		if(dirty){
			view.update_bounds();
			dirty = view.tiles.work();
			view.render();
			SDL_Flip(SDL_GetVideoSurface());
		}
	}
}

int main(int argc, char *argv[]){
	if(SDL_Init(SDL_INIT_VIDEO)){
		fprintf(stderr, "SDL_Init failed: %s", SDL_GetError());
		exit(-1);
	}
	Coordinate center = {48.4284, -123.3656};
	Point centerpt = center;
	int width = 800, height = 600;
	int zoom = 3;
	centerpt <<= zoom;
	int offsetx = centerpt.x * TILESIZE - width / 2;
	int offsety = centerpt.y * TILESIZE - height / 2;
	resize(width, height);
	MapView view(offsetx, offsety, width, height, zoom);

	runloop(view);
	return 0;
}

