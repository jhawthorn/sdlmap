#ifndef MAP_VIEW_H
#define MAP_VIEW_H MAP_VIEW_H

#include "SDL.h"
#include "tile_collection.h"

class MapView{
	public:
		TileCollection tiles;
		int zoom;
		int offsetx, offsety;
		SDL_Surface *surface;
		MapView(int offsetx, int offsety, int width, int height, int zoom);
		void zoom_at(int x, int y);
		void zoom_in();
		void zoom_out();
		void resize(int width, int height);
		void update_bounds();
		void render();
};

#endif
