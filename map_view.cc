#include "map_view.h"

MapView::MapView(int offsetx, int offsety, int width, int height, int zoom): offsetx(offsetx), offsety(offsety), zoom(zoom){
	resize(width, height);
	update_bounds();
}

void MapView::render(){
	tiles.render(offsetx, offsety);
}

void MapView::zoom_in(){
	zoom++;
	offsetx = offsetx * 2 + surface->w / 2;
	offsety = offsety * 2 + surface->h / 2;
}
void MapView::zoom_out(){
	zoom--;
	offsetx = (offsetx - surface->w / 2) / 2;
	offsety = (offsety - surface->h / 2) / 2;
}
void MapView::resize(int width, int height){
	//printf("resize(%i, %i)\n", width, height);
	surface = SDL_SetVideoMode(width, height, 0, SDL_SWSURFACE | SDL_RESIZABLE);
	if(!surface){
		fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
		exit(-1);
	}
}
void MapView::update_bounds(){
	tiles.set_bounds(offsetx / TILESIZE, offsety / TILESIZE, (offsetx + surface->w) / TILESIZE, (offsety + surface->h) / TILESIZE, zoom);
}
