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
