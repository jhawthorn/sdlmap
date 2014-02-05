#include "math.h"
#include "map_view.h"

MapView::MapView(SDL_Window *window, int width, int height, int zoom): window(window), offsetx(0), offsety(0), zoom(zoom){
	resize(width, height);
	update_bounds();
}
void MapView::center_coords(double lat, double lng){
	double x = (lng + 180.0f) / 360.0f;
	double y = (1.0f - log(tan(lat * M_PI / 180.0f) + 1.0f / cos(lat * M_PI / 180.0f)) / M_PI) / 2.0f;
	offsetx = x * (TILESIZE << zoom) - width / 2;
	offsety = y * (TILESIZE << zoom) - height / 2;
}
void MapView::render(){
	SDL_Surface *screen = SDL_GetWindowSurface(window);
	tiles.render(screen, offsetx, offsety);
	SDL_UpdateWindowSurface(window);
}
void MapView::move_by(int dx, int dy){
	offsetx += dx;
	offsety += dy;
}
void MapView::zoom_at(int x, int y){
	if(zoom < 22){
		zoom++;
		offsetx = offsetx * 2 + x;
		offsety = offsety * 2 + y;
	}
}
void MapView::zoom_in(){
	zoom_at(width / 2, height / 2);
}
void MapView::zoom_out(){
	if(zoom > 3){
		zoom--;
		offsetx = (offsetx - width / 2) / 2;
		offsety = (offsety - height / 2) / 2;
	}
}
void MapView::resize(int w, int h){
	//printf("resize(%i, %i)\n", width, height);
	width = w;
	height = h;
}
void MapView::update_bounds(){
	int maxy = (1 << zoom) * TILESIZE - height;
	if(offsety < 0)
		offsety = 0;
	else if(offsety > maxy)
		offsety = maxy;

	/* Make our x offset loop back around*/
	offsetx = mod(offsetx, TILESIZE * (1 << zoom));
	tiles.set_bounds(offsetx / TILESIZE, offsety / TILESIZE, (offsetx + width) / TILESIZE, (offsety + height) / TILESIZE, zoom);
}
