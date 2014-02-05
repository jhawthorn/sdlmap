#ifndef TILE_H
#define TILE_H TILE_H

#include <string>

#include "SDL.h"
#include "sdlmap.h"

class Tile{
	SDL_Surface *surface;
	public:
		enum State { EMPTY, ROUGH, QUEUED, LOADED } state;
		int zoom, x, y;
		Tile(int x, int y, int zoom);
		bool loaded();
		void queue();
		void load(char *memory, int size);
		std::string url();
		void render(SDL_Surface *screen, int offsetx, int offsety);

};

#endif
