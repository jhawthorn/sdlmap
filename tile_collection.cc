#include <set>

#include "SDL.h"
#include "tile_collection.h"

void TileCollection::set_bounds(int _minx, int _miny, int _maxx, int _maxy, int _zoom){
	minx = _minx;
	maxx = _maxx;
	miny = _miny;
	maxy = _maxy;
	zoom = _zoom;
	create_tiles();
}
void TileCollection::create_tiles(){
	std::set< std::pair<int, int> > existing;
	for(std::list<Tile *>::iterator it = tiles.begin(); it != tiles.end(); ++it){
		if((*it)->zoom == zoom){
			existing.insert(std::pair<int, int>((*it)->x, (*it)->y));
		}
	}

	for(int y = miny; y <= maxy; y++){
		for(int x = minx; x <= maxx; x++){
			if(existing.find(std::pair<int, int>(x, y)) == existing.end()){
				tiles.push_back(new Tile(x, y, zoom));
			}
		}
	}
};

bool TileCollection::bounded(Tile &t){
	return t.zoom == zoom && t.x >= minx && t.x <= maxx && t.y >= miny && t.y <= maxy;
}
bool TileCollection::work(){
	bool working = !transfers.empty();
	transfers.work();
	for(std::list<Tile *>::iterator it = tiles.begin(); transfers.queueable() && it != tiles.end(); ++it){
		Tile *t = *it;
		if(!bounded(*t))
			continue;
		if(t->state != Tile::EMPTY)
			continue;
		transfers.queue(t);
		working = true;
	}
	return working;
}

void TileCollection::render(int offsetx, int offsety){
	for(std::list<Tile *>::iterator it = tiles.begin(); it != tiles.end(); ++it){
		if(bounded(**it))
			(*it)->render(offsetx, offsety);
	}
	SDL_Flip(SDL_GetVideoSurface());
}

