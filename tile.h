#ifndef TILE_H
#define TILE_H TILE_H

#include "sdlmap.h"

class Tile{
	public:
		enum State { EMPTY, ROUGH, QUEUED, LOADED } state;
		int zoom, x, y;
		SDL_Surface *surface;
		Tile(int x, int y, int zoom): zoom(zoom), x(x), y(y), surface(NULL), state(EMPTY){};
		bool loaded(){
			return !!surface;
		}
		void queue(){
			state = QUEUED;
		}
		void set_surface(SDL_Surface *surface){
			this->surface = surface;
			state = LOADED;
		}
		std::string url(){
			char url[4096];
			//snprintf(url, sizeof url, "http://a.tile.openstreetmap.org/%i/%i/%i.png", zoom, x, y);
			//snprintf(url, sizeof url, "http://a.tile.stamen.com/toner/%i/%i/%i.png", zoom, x, y);
			//snprintf(url, sizeof url, "http://mts0.google.com/vt/hl=he&src=api&x=%i&s=&y=%i&z=%i", x, y, zoom);
			snprintf(url, sizeof url, "http://a.www.toolserver.org/tiles/bw-mapnik/%i/%i/%i.png", zoom, x, y);
			return std::string(url);
		}
		void render(int offsetx, int offsety){
			SDL_Rect dest = {x * TILESIZE - offsetx, y * TILESIZE - offsety, TILESIZE, TILESIZE};
			SDL_Surface *screen = SDL_GetVideoSurface();
			if(surface){
				//printf("render: (%i, %i, %i) => (%i, %i)\n", x, y, zoom, dest.x, dest.y);
				SDL_BlitSurface(surface, NULL, screen, &dest);
			}else{
				SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 255, 255, 255));
			}
		}

};

#endif
