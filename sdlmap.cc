
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

void runloop(MapView &view){
	bool mousedown = false;
	bool dirty = true;
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
							view.zoom_in();
							dirty = true;
							break;
						case 5:
							view.zoom_out();
							dirty = true;
							break;
						default:
							mousedown = true;
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
						default:
							printf("The %s key was pressed!\n", SDL_GetKeyName(event.key.keysym.sym));
							break;
					}
					break;
				case SDL_VIDEORESIZE:
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
	int zoom = 2;
	centerpt <<= zoom;
	int offsetx = centerpt.x * TILESIZE - 800 / 2;
	int offsety = centerpt.y * TILESIZE - 600 / 2;
	MapView view(offsetx, offsety, 800, 600, zoom);

	runloop(view);
	return 0;
}

