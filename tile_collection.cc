#include <set>

#include "tile_collection.h"

bool Range::contains(int x, int y, int d) const {
	assert(d <= depth);
	int dz = depth - d;
	//printf("@%i (%i,%i) <= (%i,%i) <= (%i,%i)\n", d, minx >> dz, miny >> dz, x, y, maxx >> dz, maxy >> dz);
	return x >= minx >> dz && y >= miny >> dz && x <= maxx >> dz && y <= maxy >> dz;
}

Quad::Quad(int x, int y, int depth): x(x), y(y), depth(depth){
	tile = NULL;
	for(int i = 0; i < 4; i++)
		children[i] = NULL;
}
void Quad::queryRange(std::list<Tile *> &l, const Range &r){
	if(!r.contains(x, y, depth))
		return;

	if(!tile)
		tile = new Tile(x, y, depth);

	if(depth < r.depth){
		buildChildren();
		for(int i = 0; i < 4; i++)
			children[i]->queryRange(l, r);
	}else{
		l.push_back(tile);
	}
}
void Quad::buildChildren(){
	if(!children[0])
		children[0] = new Quad(x*2,   y*2, depth+1);
	if(!children[1])
		children[1] = new Quad(x*2+1, y*2, depth+1);
	if(!children[2])
		children[2] = new Quad(x*2,   y*2+1, depth+1);
	if(!children[3])
		children[3] = new Quad(x*2+1, y*2+1, depth+1);
}

void TileCollection::set_bounds(int _minx, int _miny, int _maxx, int _maxy, int _zoom){
	range.minx = _minx;
	range.maxx = _maxx;
	range.miny = _miny;
	range.maxy = _maxy;
	range.depth = _zoom;
}

bool TileCollection::work(){
	bool working = !!transfers.active();
	transfers.work();

	std::list<Tile *> l;
	quad.queryRange(l, range);

	for(std::list<Tile *>::iterator it = l.begin(); transfers.active() < 2 && it != l.end(); ++it){
		Tile *t = *it;
		if(t->state != Tile::EMPTY)
			continue;
		transfers.queue(t);
		working = true;
	}
	return working;
}

void TileCollection::render(SDL_Surface *screen, int offsetx, int offsety){
	std::list<Tile *> l;
	quad.queryRange(l, range);
	for(std::list<Tile *>::iterator it = l.begin(); it != l.end(); ++it){
		(*it)->render(screen, offsetx, offsety);
	}
}

