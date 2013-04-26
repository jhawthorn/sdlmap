#include "map_view.h"

MapView::MapView(int offsetx, int offsety, int width, int height, int zoom): offsetx(offsetx), offsety(offsety), zoom(zoom){
	resize(width, height);
	update_bounds();
}

void MapView::render(){
	tiles.render(offsetx, offsety);
}

void MapView::zoom_at(int x, int y){
	if(zoom < 22){
		zoom++;
		offsetx = offsetx * 2 + x;
		offsety = offsety * 2 + y;
	}
}
void MapView::zoom_in(){
	zoom_at(surface->w / 2, surface->h / 2);
}
void MapView::zoom_out(){
	if(zoom > 3){
		zoom--;
		offsetx = (offsetx - surface->w / 2) / 2;
		offsety = (offsety - surface->h / 2) / 2;
	}
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
	int maxy = (1 << zoom) * TILESIZE - surface->h;
	if(offsety < 0)
		offsety = 0;
	else if(offsety > maxy)
		offsety = maxy;

	/* Make our x offset loop back around*/
	offsetx = mod(offsetx, TILESIZE * (1 << zoom));
	tiles.set_bounds(offsetx / TILESIZE, offsety / TILESIZE, (offsetx + surface->w) / TILESIZE, (offsety + surface->h) / TILESIZE, zoom);
}
