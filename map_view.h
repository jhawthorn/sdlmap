#ifndef MAP_VIEW_H
#define MAP_VIEW_H MAP_VIEW_H

#include "tile_collection.h"

class Coordinate;
class MapView{
	public:
		TileCollection tiles;
		int zoom;
		int offsetx, offsety;
		SDL_Surface *surface;
		MapView(int width, int height, Coordinate &center, int zoom);
		void zoom_in(){
			zoom++;
			offsetx = offsetx * 2 + surface->w / 2;
			offsety = offsety * 2 + surface->h / 2;
		}
		void zoom_out(){
			zoom--;
			offsetx = (offsetx - surface->w / 2) / 2;
			offsety = (offsety - surface->h / 2) / 2;
		}
		void resize(int width, int height){
			//printf("resize(%i, %i)\n", width, height);
			surface = SDL_SetVideoMode(width, height, 0, SDL_SWSURFACE | SDL_RESIZABLE);
			if(!surface){
				fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
				exit(-1);
			}
		}
		void update_bounds(){
			tiles.set_bounds(offsetx / TILESIZE, offsety / TILESIZE, (offsetx + surface->w) / TILESIZE, (offsety + surface->h) / TILESIZE, zoom);
		}
		void render();
};

#endif
